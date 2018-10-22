using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using UnityL10nToolCppCLI;

namespace UnityL10nToolPatcherCShop
{
    /// <summary>
    /// MainWindow.xaml에 대한 상호 작용 논리
    /// </summary>
    public partial class MainWindow : Window
    {
        UnityL10nToolCppManaged unityL10nToolCppManaged;

        public MainWindow()
        {
            InitializeComponent();
        }

        private void Button_Click(object sender, RoutedEventArgs e)
        {
            unityL10nToolCppManaged = new UnityL10nToolCppManaged(".\\");
            unityL10nToolCppManaged.LoadGlobalgamemanagersFile();
            unityL10nToolCppManaged.ProcessResourceAndMonoManger();
            unityL10nToolCppManaged.LoadMonoClassDatabase();
            unityL10nToolCppManaged.LoadUnityL10nToolAPI();
            unityL10nToolCppManaged.DownloadResourcesFromInternetToTempFolder();
            List<string> loadedFontPlugins = unityL10nToolCppManaged.LoadFontPlugins();
            unityL10nToolCppManaged.SetPacherConfigJson();
            unityL10nToolCppManaged.LoadAssetsReplacer();

            unityL10nToolCppManaged.LoadTextPlugins();
            unityL10nToolCppManaged.GetTextAssetMaps();
            unityL10nToolCppManaged.GetTranslatedTextAssetsFromFile();
            unityL10nToolCppManaged.GetAssetReplacerFromTextAssets();


            unityL10nToolCppManaged.MakeModifiedAssetsFile();
            unityL10nToolCppManaged.CopyResourceFileToGameFolder();
        }
    }
}
