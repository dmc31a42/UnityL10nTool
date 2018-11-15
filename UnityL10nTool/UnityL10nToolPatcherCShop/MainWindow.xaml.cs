using Microsoft.WindowsAPICodePack.Dialogs;
using Newtonsoft.Json.Linq;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.IO;
using System.Linq;
using System.Net;
using System.Text;
using System.Threading.Tasks;
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
        JObject patcherJson;
        //public string GameFolderPath { get; set; }
        //public static readonly DependencyProperty GameFolderPathProperty
        //    = DependencyProperty.Register(
        //        "GameFolderPath",
        //        typeof(string),
        //        typeof(MainWindow),
        //        new PropertyMetadata(""));
        string GameName { get; set; }
        string MakerName { get; set; }
        string DataFolderName { get; set; }
        BackgroundWorker PatchBackgroundWorker;
        List<OnlineResourcePairCLI> OnlineResourcePairCLIs = new List<OnlineResourcePairCLI>();
        public MainWindow()
        {
            DataContext = this;
            InitializeComponent();
            string jsonStr = System.IO.File.ReadAllText("setting.json");
            patcherJson = JObject.Parse(jsonStr);
            DataFolderName = (string)patcherJson["DataFolderName"];
            GameName = (string)patcherJson["GameName"];
            MakerName = (string)patcherJson["MakerName"];
            Title = GameName + " Patcher";
            gameFolderPathTextBox.Text = UnityL10nToolCppManaged.FindUnityGameFolderFromDataFolderName(DataFolderName, GameName, MakerName);
            if (File.Exists(AppDomain.CurrentDomain.BaseDirectory + "customMain.png"))
            {
                Uri uri = new Uri(AppDomain.CurrentDomain.BaseDirectory + "customMain.png", UriKind.RelativeOrAbsolute);
                MainImage.Source = BitmapFrame.Create(uri);
            }
            else
            {

            }
        }

        private void BackgroundWorker_DoWork1(object sender, DoWorkEventArgs e)
        {
            throw new NotImplementedException();
        }

        private void Button_Click(object sender, RoutedEventArgs e)
        {
            if((ErrorMessageTextBlock.Visibility == Visibility.Visible))
            {
                if(MessageBox.Show("Game Folder is not corrected.\nPlease check Game Folder setting.\nDo you continue anyway?","Incorrect Game Folder", MessageBoxButton.YesNo) == MessageBoxResult.No)
                {
                    return;
                }
            }
            PatchButton.IsEnabled = false;
            if (patcherJson.ContainsKey("OnlineResources"))
            {
                JToken[] jTokens = patcherJson["OnlineResources"].ToArray();
                foreach (JToken jToken in jTokens)
                {
                    string URL = (string)jToken["URL"];
                    string filePath = (String)jToken["filePath"];
                    if (URL != null && filePath != null)
                    {
                        OnlineResourcePairCLI onlineResourcePairCLI = new OnlineResourcePairCLI
                        {
                            URL = URL,
                            filePath = filePath
                        };
                        OnlineResourcePairCLIs.Add(onlineResourcePairCLI);
                    }
                }
            }
            System.IO.DirectoryInfo di = new System.IO.DirectoryInfo("temp\\");
            // Delete this dir and all subdirs.
            try
            {
                di.Delete(true);
            }
            catch (System.IO.IOException ioException)
            {
                Console.WriteLine(ioException.Message);
            }
            System.IO.Directory.CreateDirectory("temp\\");
            PatchBackgroundWorker = new BackgroundWorker();
            PatchBackgroundWorker.DoWork += BackgroundWorker_DoWork;
            PatchBackgroundWorker.ProgressChanged += PatchBackgroundWorker_ProgressChanged;
            PatchBackgroundWorker.RunWorkerCompleted += PatchBackgroundWorker_RunWorkerCompleted;
            PatchBackgroundWorker.WorkerReportsProgress = true;
            PatchBackgroundWorker.RunWorkerAsync(gameFolderPathTextBox.Text);

            
        }

        private void PatchBackgroundWorker_RunWorkerCompleted(object sender, RunWorkerCompletedEventArgs e)
        {
            MessageBox.Show("Patch complete.\nPatcher will be closed.", "Patch complete");
            Window.GetWindow(this).Close();
        }

        int maximumProgress = 16;
        int currentProgress = 0;
        private void PatchBackgroundWorker_ProgressChanged(object sender, ProgressChangedEventArgs e)
        {
            currentProgress += e.ProgressPercentage;
            ProgressListBox.Items.Add((string)e.UserState);
            ProgressBar1.Value = (int)(currentProgress * 100 / maximumProgress);
            ProgressListBox.SelectedIndex = ProgressListBox.Items.Count - 1;
            ProgressListBox.ScrollIntoView(ProgressListBox.SelectedItem);

        }

        private async void BackgroundWorker_DoWork(object sender, DoWorkEventArgs e)
        {
            string gamePath = (string)e.Argument;
            PatchBackgroundWorker.ReportProgress(1, "Loading patcher configuration...");
            unityL10nToolCppManaged = new UnityL10nToolCppManaged(".\\", gamePath);
            PatchBackgroundWorker.ReportProgress(1, "Downloading resources...");
            List<Task> downloadTasksQuery = new List<Task>();
            foreach (OnlineResourcePairCLI onlineResourcePairCLI in OnlineResourcePairCLIs)
            {
                WebClient webClient = new WebClient();
                // https://stackoverflow.com/questions/47576074/get-releases-github-api-v3
                webClient.Headers.Add("User-Agent", "UnityL10nTool");
                downloadTasksQuery.Add(webClient.DownloadFileTaskAsync(onlineResourcePairCLI.URL, "temp\\" + onlineResourcePairCLI.filePath));
            }
            Task[] downloadTasks = downloadTasksQuery.ToArray();
            PatchBackgroundWorker.ReportProgress(1, "Loading globalgamemanagers...");
            unityL10nToolCppManaged.LoadGlobalgamemanagersFile();
            PatchBackgroundWorker.ReportProgress(1, "Loading Resource and MonoManager...");
            unityL10nToolCppManaged.ProcessResourceAndMonoManger();
            PatchBackgroundWorker.ReportProgress(1, "Loading mono ClassDatabase...");
            unityL10nToolCppManaged.LoadMonoClassDatabase();
            PatchBackgroundWorker.ReportProgress(1, "Loading UnityL10nTool API...");
            unityL10nToolCppManaged.LoadUnityL10nToolAPI();
            await Task.WhenAll(downloadTasks);
            PatchBackgroundWorker.ReportProgress(1, "Downloading resources...");
            //unityL10nToolCppManaged.GetOnlineResourcePairs();
            //unityL10nToolCppManaged.DownloadResourcesFromInternetToTempFolder();
            PatchBackgroundWorker.ReportProgress(1, "Loading font plugins...");
            List<string> loadedFontPlugins = unityL10nToolCppManaged.LoadFontPlugins();
            PatchBackgroundWorker.ReportProgress(1, "Setting patcher configuration...");
            unityL10nToolCppManaged.SetPacherConfigJson();
            PatchBackgroundWorker.ReportProgress(1, "Loading font assets replacers...");
            unityL10nToolCppManaged.LoadAssetsReplacer();
            PatchBackgroundWorker.ReportProgress(1, "Loading text plugins...");
            unityL10nToolCppManaged.LoadTextPlugins();
            PatchBackgroundWorker.ReportProgress(1, "Loading text asset maps...");
            unityL10nToolCppManaged.GetTextAssetMaps();
            PatchBackgroundWorker.ReportProgress(1, "Loading translated text assets from file...");
            unityL10nToolCppManaged.GetTranslatedTextAssetsFromFile();
            PatchBackgroundWorker.ReportProgress(1, "Loading text assets replacers...");
            unityL10nToolCppManaged.GetAssetReplacerFromTextAssets();
            PatchBackgroundWorker.ReportProgress(1, "Making modified assets files...");
            unityL10nToolCppManaged.MakeModifiedAssetsFile();
            PatchBackgroundWorker.ReportProgress(1, "Copying resource files to game folder...");
            unityL10nToolCppManaged.CopyResourceFileToGameFolder();
            unityL10nToolCppManaged.GetCopyResourcesManualPairs();
            unityL10nToolCppManaged.CopyResourcesManualPairsForPatcher();
            PatchBackgroundWorker.ReportProgress(1, "Done...");
        }

        private void gameFolderPathTextBox_Pasting(object sender, DataObjectPastingEventArgs e)
        {
            var isText = e.SourceDataObject.GetDataPresent(DataFormats.UnicodeText, true);
            if (!isText) return;

            var text = e.SourceDataObject.GetData(DataFormats.UnicodeText) as string;
            if (text.Last() != '\\')
            {
                e.CancelCommand();
                gameFolderPathTextBox.Text = text+ '\\';
                return;
            }

            if (UnityL10nToolCppManaged.DetermineProjectGamePath(text, GameName, MakerName))
            {
                ErrorMessageTextBlock.Visibility = Visibility.Hidden;
            } else
            {
                gameFolderPathTextBox.Text += text + DataFolderName + "_Data\\";
                return;
            }
        }

        private void gameFolderPathTextBox_TextChanged(object sender, TextChangedEventArgs e)
        {
            string tempPath = gameFolderPathTextBox.Text;
            if (gameFolderPathTextBox.Text.Last() != '\\')
            {
                gameFolderPathTextBox.Text += '\\';
                return;
            }
            if (UnityL10nToolCppManaged.DetermineProjectGamePath(gameFolderPathTextBox.Text, GameName, MakerName))
            {
                ErrorMessageTextBlock.Visibility = Visibility.Hidden;
            }
            else
            {
                string tempStr = gameFolderPathTextBox.Text + DataFolderName + "_Data\\";
                if(UnityL10nToolCppManaged.DetermineProjectGamePath(tempStr, GameName, MakerName))
                {
                    gameFolderPathTextBox.Text += DataFolderName + "_Data\\";
                    ErrorMessageTextBlock.Visibility = Visibility.Hidden;
                } else
                {
                    ErrorMessageTextBlock.Visibility = Visibility.Visible;
                }
                return;
            }
        }

        private void SelectGameFolderButton_Click(object sender, RoutedEventArgs e)
        {
            var dialog = new CommonOpenFileDialog
            {
                EnsurePathExists = true,
                EnsureFileExists = false,
                AllowNonFileSystemItems = false,
                IsFolderPicker = true,
                Title = "Select Game Folder"
            };
            if(gameFolderPathTextBox.Text!="")
            {
                dialog.InitialDirectory = gameFolderPathTextBox.Text;
            }

            if (dialog.ShowDialog() == CommonFileDialogResult.Ok)
            {
                string dirToProcess = Directory.Exists(dialog.FileName) ? dialog.FileName : System.IO.Path.GetDirectoryName(dialog.FileName);
                gameFolderPathTextBox.Text = dirToProcess;
            }
        }
    }
}
