// TBD copyrights stuffs

using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Numerics;
using System.Runtime.InteropServices.WindowsRuntime;
using System.Threading.Tasks;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.System.Threading;
using Windows.UI.Core;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;
using Windows.Foundation.Collections;
using System.Threading;
using Windows.Foundation.Metadata;
using Windows.UI.ViewManagement;
using cvRT;
using static Windows.System.Threading.ThreadPool;
using Point = cvRT.Point;
using Rect = cvRT.Rect;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=402352&clcid=0x409

namespace highgui_xaml.WindowsCSharp
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class MainPage : Page
    {
        /// <summary>
        /// 
        /// </summary>
        private readonly VideoIo _videoIo;

        private int _processingMethodIndex = 4;

        /// <summary>
        /// 
        /// </summary>
        public MainPage()
        {
            this.InitializeComponent();

            _videoIo = new VideoIo();
            _videoIo.Initialize();
            _videoIo.SetImage(OCVImage);

            RunAsync(asyncAction => CvMainThread());
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void comboBox_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            var cb = (ComboBox)sender;

            _processingMethodIndex = cb.SelectedIndex;
            var selectedItem = ((ComboBoxItem)cb.SelectedItem).Content.ToString();

            Debug.WriteLine(selectedItem);

            // TBD[stjong] change the processing here.
        }

        /// <summary>
        /// Worker thread for image processing.
        /// </summary>
        public void CvMainThread()
        {
            var faceCascade = new CascadeClassifier();
            var eyesCascade = new CascadeClassifier();

            faceCascade.Load("haarcascade_frontalface_alt.xml");
            eyesCascade.Load("haarcascade_eye_tree_eyeglasses.xml");
                

            // TBD need exit event.

            _videoIo.StartCapture();

            while (true)
            {
                var srcFrame = new Mat();
                var dstFrame = new Mat();
                _videoIo.GetFrame(srcFrame);

                switch (_processingMethodIndex)
                {
                    // passthrough
                    case 0:
                        break;
                    
                    // gray
                    case 1:
                        ImgProc.cvtColor(srcFrame, dstFrame, ColorConversionCodes.COLOR_RGBA2GRAY);
                        ImgProc.cvtColor(dstFrame, srcFrame, ColorConversionCodes.COLOR_GRAY2RGB);
                        break;
                    
                    // canny
                    case 3:
                        ImgProc.cvtColor(srcFrame, dstFrame, cvRT.ColorConversionCodes.COLOR_RGBA2GRAY);
                        ImgProc.GaussianBlur(dstFrame, dstFrame, new cvRT.Size(7, 7), 1.5, 1.5);
                        ImgProc.Canny(dstFrame, dstFrame, 0, 30, 3);
                        ImgProc.cvtColor(dstFrame, srcFrame, ColorConversionCodes.COLOR_GRAY2RGB);
                        break;

                    // contour
                    case 4:
                    {
                        var contours = new List<IList<Point>>();
                        var hierarchy = new List<Vec4i>();
                        
                        const int thresh = 100;
                        // RNG rng = new RNG(12345);
                        
                        ImgProc.Canny(srcFrame, dstFrame, thresh, thresh * 2, 3);
                        ImgProc.FindContours(dstFrame, contours, hierarchy, ContourRetrievalAlgorithm.RETR_TREE, ContourApproximationModes.CHAIN_APPROX_SIMPLE, new Point(0, 0));

                        // dstFrame = new Scalar(0, 0, 0, 0);

                        for (var i = 0 ; i < contours.Count();  i++)
                        {
                            var randGen = new Random();

                            var color = new Scalar(randGen.Next(0, 255), randGen.Next(0, 255), randGen.Next(0, 255), randGen.Next(0, 255));

                            ImgProc.DrawContours(srcFrame, contours, i, color, 2, 8, hierarchy, 0, new Point(0, 0));
                        }
                            
                        break;
                    }

                    // face detect
                    case 5:
                    {
                        ImgProc.cvtColor(srcFrame, dstFrame, ColorConversionCodes.COLOR_RGBA2GRAY);
                        ImgProc.EqualizeHist(dstFrame, dstFrame);

                        // Faces in the frame.
                        var faces = new List<Rect>();

                        try
                        {
                            faces = new List<Rect>();
                            faceCascade.detectMultiScale(dstFrame, faces, 1.1, 2, (int)(0 | CV_HAAR.SCALE_IMAGE), new cvRT.Size(30, 30));
                        }
                        catch (Exception ex)
                        {
                            Debug.WriteLine("Exception {0}", ex.Message);
                        }

                        // For each face, detect the eyes
                        foreach (var face in faces)
                        {
                            // Draw ellipse for the face.
                            var faceCenter = new Point(face.X + face.Width / 2, face.Y + face.Height / 2);
                            ImgProc.Ellipse(srcFrame, faceCenter, new cvRT.Size(face.Width / 2, face.Height / 2), 0, 0, 360, new Scalar(255, 0, 255, 0), 4, 8, 0);

                            // Detect the eyes for the face
                            var faceRoi = dstFrame.RectSubMettric(face);
                            var eyes = new List<Rect>();
                            eyesCascade.detectMultiScale(faceRoi, eyes, 1.1, 2, (int) (0 | CASCADE.CASCADE_SCALE_IMAGE),new cvRT.Size(30, 30));
                            
                            // Draw the eyes
                            foreach (var eye in eyes)
                            {
                                var eyeCenter = new Point(face.X + eye.X + eye.Width/2, face.Y + eye.Y + eye.Height/2);
                                var radius = (int) Math.Round((eye.Width + eye.Height) * 0.25);
                                ImgProc.Circle(srcFrame, eyeCenter, radius, new Scalar(255, 0, 0, 0), 4, 8, 0);
                            }
                       }

                        break;
                    }

                    default:
                        break;
                }
                _videoIo.ShowFrame(srcFrame);
                Task.Delay(TimeSpan.FromMilliseconds(500));
            }
        }
    }
}