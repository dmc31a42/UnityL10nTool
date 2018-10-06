using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;
using UnityL10nToolCppCLI;

namespace UnityL10nToolCShop
{
    /// <summary>
    /// ProjectConfig.xaml에 대한 상호 작용 논리
    /// </summary>
    public partial class ProjectConfig : Window
    {
        UnityL10nToolCppManaged unityL10nToolCppManaged;
        UnityL10nToolProjectInfo unityL10NToolProjectInfo;
        BackgroundWorker LoadUnityL10nTool_BackgroundWorker;
        ProjectConfigSplash projectConfigSplash;
        Dictionary<string, FontAssetMapsCLI> pluginsSupportAssetMap;
        public FontAssetMapCLI SelectedFontAssetItem;

        public ProjectConfig(UnityL10nToolProjectInfo unityL10NToolProjectInfo)
        {
            InitializeComponent();
            this.unityL10NToolProjectInfo = unityL10NToolProjectInfo;
            GameNameLabel.Content = unityL10NToolProjectInfo.GameName;
            MakerNameLabel.Content = unityL10NToolProjectInfo.MakerName;
            GamePathTextBlock.Text = unityL10NToolProjectInfo.GamePath;
        }

        private void Window_Loaded(object sender, RoutedEventArgs e)
        {
            projectConfigSplash = new ProjectConfigSplash();
            projectConfigSplash.SetValue(Grid.RowSpanProperty, 2);
            projectConfigSplash.SetValue(Grid.ColumnSpanProperty, 2);

            MainGrid.Children.Add(projectConfigSplash);
            LoadUnityL10nTool_BackgroundWorker = new BackgroundWorker();
            LoadUnityL10nTool_BackgroundWorker.DoWork += LoadUnityL10nTool_DoWork;
            LoadUnityL10nTool_BackgroundWorker.ProgressChanged += LoadUnityL10nTool_ProgressChanged;
            LoadUnityL10nTool_BackgroundWorker.RunWorkerCompleted += LoadUnityL10nTool_RunWorkerCompleted;
            LoadUnityL10nTool_BackgroundWorker.WorkerReportsProgress = true;
            LoadUnityL10nTool_BackgroundWorker.RunWorkerAsync();
            //LoadUnityL10nTool_Test();
        }

        private void LoadUnityL10nTool_Test()
        {
            string gamePath = "";
            // DeterminateUnityGameFolder말고 GameName과 MakerName으로 app.info에 있는 정보를 통해서 본 프로젝트(게임)의 폴더가 맞는가 확인해야함.
            if (UnityL10nToolCppManaged.DetermineProjectGamePath(
                unityL10NToolProjectInfo.GamePath,
                unityL10NToolProjectInfo.GameName,
                unityL10NToolProjectInfo.MakerName))
            {
                gamePath = unityL10NToolProjectInfo.GamePath;
            }
            if (gamePath == "")
            {
                gamePath = UnityL10nToolCppManaged.FindUnityGameFolderFromDataFolderName(
                   unityL10NToolProjectInfo.DataFolderName,
                   unityL10NToolProjectInfo.GameName,
                   unityL10NToolProjectInfo.MakerName);
            }
            if (gamePath == "")
            {
                // 게임폴더가 비정상일때 (자신의 경로라도 .\로 넘어옴
            }
            unityL10NToolProjectInfo.GamePath = gamePath;

            unityL10nToolCppManaged = new UnityL10nToolCppManaged(unityL10NToolProjectInfo.GamePath);

            unityL10nToolCppManaged.LoadGlobalgamemanagersFile();

            unityL10nToolCppManaged.LoadMonoClassDatabase();

            unityL10nToolCppManaged.LoadUnityL10nToolAPI();

            List<string> loadedFontPlugins = unityL10nToolCppManaged.LoadFontPlugins();

            pluginsSupportAssetMap = unityL10nToolCppManaged.GetPluginsSupportAssetMap();

            MainGrid.Children.Remove(projectConfigSplash);
            GamePathTextBlock.Text = unityL10NToolProjectInfo.GamePath;
            FontAssetTabControl.ItemsSource = pluginsSupportAssetMap;
        }

        private void LoadUnityL10nTool_DoWork(object sender, DoWorkEventArgs e)
        {
            string gamePath = "";
            // DeterminateUnityGameFolder말고 GameName과 MakerName으로 app.info에 있는 정보를 통해서 본 프로젝트(게임)의 폴더가 맞는가 확인해야함.
            if (UnityL10nToolCppManaged.DetermineProjectGamePath(
                unityL10NToolProjectInfo.GamePath,
                unityL10NToolProjectInfo.GameName,
                unityL10NToolProjectInfo.MakerName))
            {
                gamePath = unityL10NToolProjectInfo.GamePath;
            }
            if (gamePath == "")
            {
                gamePath = UnityL10nToolCppManaged.FindUnityGameFolderFromDataFolderName(
                   unityL10NToolProjectInfo.DataFolderName,
                   unityL10NToolProjectInfo.GameName,
                   unityL10NToolProjectInfo.MakerName);
            }
            if (gamePath == "")
            {
                // 게임폴더가 비정상일때 (자신의 경로라도 .\로 넘어옴
            }
            unityL10NToolProjectInfo.GamePath = gamePath;

            LoadUnityL10nTool_BackgroundWorker.ReportProgress(0, "Initialize UnityL10nTool...");
            unityL10nToolCppManaged = new UnityL10nToolCppManaged(unityL10NToolProjectInfo.GamePath);

            LoadUnityL10nTool_BackgroundWorker.ReportProgress(0, "Loading Assets...");
            unityL10nToolCppManaged.LoadGlobalgamemanagersFile();

            LoadUnityL10nTool_BackgroundWorker.ReportProgress(0, "Loading MonoClassDatabase...");
            unityL10nToolCppManaged.LoadMonoClassDatabase();

            LoadUnityL10nTool_BackgroundWorker.ReportProgress(0, "Loading UnityL10nToolAPI...");
            unityL10nToolCppManaged.LoadUnityL10nToolAPI();

            LoadUnityL10nTool_BackgroundWorker.ReportProgress(0, "Loading Font Plugins...");
            List<string> loadedFontPlugins = unityL10nToolCppManaged.LoadFontPlugins();

            LoadUnityL10nTool_BackgroundWorker.ReportProgress(0, "Loading Plugins Support Asset List...");
            pluginsSupportAssetMap = unityL10nToolCppManaged.GetPluginsSupportAssetMap();
        }

        private void LoadUnityL10nTool_ProgressChanged(object sender, ProgressChangedEventArgs e)
        {
            projectConfigSplash.ProgressText = (string)e.UserState;
        }

        private void LoadUnityL10nTool_RunWorkerCompleted(object sender, RunWorkerCompletedEventArgs e)
        {
            MainGrid.Children.Remove(projectConfigSplash);
            GamePathTextBlock.Text = unityL10NToolProjectInfo.GamePath;
            FontAssetTabControl.ItemsSource = pluginsSupportAssetMap;
            LoadUnityL10nTool_BackgroundWorker.Dispose();
        }

        private void Window_Closed(object sender, EventArgs e)
        {
            Console.WriteLine("test1");
            unityL10nToolCppManaged.Dispose();
            unityL10nToolCppManaged = null;
            //App.Current.MainWindow.Close();
            Console.WriteLine("test2");
        }

        private void Button_Click(object sender, RoutedEventArgs e)
        {
            Button button = (Button)sender;
            FontAssetMapCLI fontAssetMapCLI = (FontAssetMapCLI)(button).DataContext;
            Grid grid = (Grid)button.Parent;
            Grid grid2 = (Grid)grid.Parent;
            // 카피해서 넣어야함.
            FontAssetMapsCLI fontAssetMapsCLI = (FontAssetMapsCLI)grid2.DataContext;
            // 중복 확인.
            fontAssetMapsCLI.Saveds.Add(fontAssetMapCLI);

        }

        private void Button_Click_1(object sender, RoutedEventArgs e)
        {
            // Edit하면 command로 SelectedItem을 바꾸고,  Add 버튼을 Edit 버튼으로 바꾸기? 
        }
    }
}
