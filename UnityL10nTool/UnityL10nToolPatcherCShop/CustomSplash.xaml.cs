using Newtonsoft.Json.Linq;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;

namespace UnityL10nToolPatcherCShop
{
    /// <summary>
    /// CustomSplash.xaml에 대한 상호 작용 논리
    /// </summary>
    public partial class CustomSplash : Window
    {
        public JObject patcherJson;
        public CustomSplash()
        {
            InitializeComponent();
            string jsonStr = System.IO.File.ReadAllText("setting.json");
            patcherJson = JObject.Parse(jsonStr);
            if (File.Exists(AppDomain.CurrentDomain.BaseDirectory + "splash.png"))
            {
                Uri uri = new Uri(AppDomain.CurrentDomain.BaseDirectory + "splash.png", UriKind.RelativeOrAbsolute);
                SplashImage.Source = BitmapFrame.Create(uri);
            }
        }

        private async void WailtFor2Second()
        {
            await Task.Delay(2000);
        }

        private void Window_Loaded(object sender, RoutedEventArgs e)
        {
            if (patcherJson.ContainsKey("OnlineUpdate"))
            {
                JToken onlineUpdateJson = patcherJson["OnlineUpdate"];

            }
        }
    }
}
