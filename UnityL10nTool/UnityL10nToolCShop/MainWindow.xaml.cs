using Microsoft.WindowsAPICodePack.Dialogs;
using Newtonsoft.Json.Linq;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.IO;
using System.Linq;
using System.Net;
using System.Reflection;
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

namespace UnityL10nToolCShop
{
    /// <summary>
    /// MainWindow.xaml에 대한 상호 작용 논리
    /// </summary>
    public partial class MainWindow : Window
    {
        List<string> projectList = new List<string>();
        //public ObservableCollection<UnityL10nToolProjectInfo> UnityL10nToolProjectInfos { get; private set; }
        int majorVersion;
        int minorVersion;
        int BuildVersion;
        string VersionString;

        public MainWindow()
        {
            InitializeComponent();
            System.Net.ServicePointManager.SecurityProtocol = System.Net.SecurityProtocolType.Tls12;
            // currentVersion
            string strVersionText = Assembly.GetExecutingAssembly().FullName
                        .Split(',')[1]
                        .Trim()
                        .Split('=')[1];
            majorVersion = Convert.ToInt32(strVersionText.Split('.')[0]);
            minorVersion = Convert.ToInt32(strVersionText.Split('.')[1]);
            BuildVersion = Convert.ToInt32(strVersionText.Split('.')[2]);
            VersionString = 'v' + majorVersion.ToString() + '.' + minorVersion.ToString() + '.' + BuildVersion.ToString();

            ObservableCollection<UnityL10nToolProjectInfo>  tempUnityL10nToolProjectInfos = new ObservableCollection<UnityL10nToolProjectInfo>();
            // http://infodbbase.tistory.com/105
            if (System.IO.Directory.Exists("Projects"))
            {
                System.IO.DirectoryInfo di = new System.IO.DirectoryInfo("Projects");

                foreach(var directory in di.GetDirectories())
                {
                    FileInfo[] settingFileInfos = directory.GetFiles("setting.json");
                    if(settingFileInfos.Length == 1)
                    {
                        string jsonStr = System.IO.File.ReadAllText(settingFileInfos[0].FullName);
                        JObject jObject = JObject.Parse(jsonStr);
                        string GameName = (string)jObject["GameName"];
                        string MakerName = (string)jObject["MakerName"];
                        string GamePath = (string)jObject["GamePath"];
                        string DataFolderName = (string)jObject["DataFolderName"];
                        tempUnityL10nToolProjectInfos.Add(new UnityL10nToolProjectInfo(
                            GameName:GameName, 
                            MakerName:MakerName, 
                            GamePath:GamePath, 
                            JSONPath:settingFileInfos[0].FullName,
                            DataFolderName: DataFolderName));
                    }
                }
                ProjectListBox.ItemsSource = tempUnityL10nToolProjectInfos;
            }

            if (projectList.Count == 0)
            {

            } else
            {

            }
        }

        private void Button_Click(object sender, RoutedEventArgs e)
        {
            // https://stackoverflow.com/questions/11624298/how-to-use-openfiledialog-to-select-a-folder
            // 유니티의 폴더 열기를 구현할 수 있는 방법이 있는것 같음

            //UnityL10nToolCppManaged unityL10NToolCppManaged = ((App)Application.Current).unityL10NToolCppManaged = new UnityL10nToolCppManaged(gameFolderPath.Text);
            //Dictionary<string, List<FontAssetMapCLI>> directory = unityL10NToolCppManaged.GetPluginsSupportAssetMap();
            var dialog = new CommonOpenFileDialog
            {
                EnsurePathExists = true,
                EnsureFileExists = false,
                AllowNonFileSystemItems = false,
                IsFolderPicker = true,
                Title = "Select Unity Game Folder"
            };

            if (dialog.ShowDialog() == CommonFileDialogResult.Ok)
            {
                string dirToProcess = Directory.Exists(dialog.FileName) ? dialog.FileName : System.IO.Path.GetDirectoryName(dialog.FileName);
                string projectJsonFolder = UnityL10nToolCppCLI.UnityL10nToolCppManaged.NewGameProjectFromFolder(dirToProcess);
                if(projectJsonFolder != "")
                {
                    string jsonStr = System.IO.File.ReadAllText(projectJsonFolder + "setting.json");
                    JObject jObject = JObject.Parse(jsonStr);
                    string GameName = (string)jObject["GameName"];
                    string MakerName = (string)jObject["MakerName"];
                    string GamePath = (string)jObject["GamePath"];
                    string DataFolderName = (string)jObject["DataFolderName"];
                    UnityL10nToolProjectInfo unityL10NToolProjectInfo = new UnityL10nToolProjectInfo(
                        GameName: GameName,
                        MakerName: MakerName,
                        GamePath: GamePath,
                        JSONPath: projectJsonFolder + "setting.json",
                        DataFolderName: DataFolderName);
                    Window projectConfig = new ProjectConfig(unityL10NToolProjectInfo);
                    projectConfig.Show();
                    Window.GetWindow(this).Close();
                } else
                {
                    MessageBox.Show("Fail to find unity game folder. please check github wiki manual. If there is a problem, please report issue at github.", "Find unity folder fail");
                }
            }
        }

        private void ProjectListBox_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            UnityL10nToolProjectInfo unityL10NToolProjectInfo = (ProjectListBox.SelectedItem as UnityL10nToolProjectInfo);
            if(unityL10NToolProjectInfo != null)
            {
                Window projectConfig = new ProjectConfig(unityL10NToolProjectInfo);
                projectConfig.Show();
                Window.GetWindow(this).Close();
            }
        }

        private void Window_Loaded(object sender, RoutedEventArgs e)
        {
            try
            {
                WebClient webClient = new WebClient();
                webClient.Headers.Add("User-Agent", "UnityL10nTool");
                string gitHubApiURL = "https://api.github.com/repos/dmc31a42/UnityL10nTool/releases/latest";
                string gitHubAPIResult = webClient.DownloadString(new Uri(gitHubApiURL));
                JObject gitHubJson = JObject.Parse(gitHubAPIResult);
                if (gitHubJson.ContainsKey("tag_name") && gitHubJson.ContainsKey("assets"))
                {
                    string currentVersionDownload = gitHubJson["tag_name"].ToObject<string>();
                    string lastestVersionStr = currentVersionDownload;
                    if (VersionString != lastestVersionStr)
                    {
                        if (MessageBox.Show(
                            "Lastest Version is updated. Click 'Yes' to visit GitHub release page and close this program.",
                            "Lastest Version exist",
                            MessageBoxButton.YesNo) == MessageBoxResult.Yes)
                        {
                            System.Diagnostics.Process.Start("https://github.com/dmc31a42/UnityL10nTool/releases/latest");
                            Window.GetWindow(this).Close();
                        }
                    }
                }
            } catch (Exception ex)
            {

            }
            
        }
    }
}
