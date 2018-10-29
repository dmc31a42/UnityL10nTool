using Microsoft.WindowsAPICodePack.Dialogs;
using Newtonsoft.Json.Linq;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.IO;
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
        }

        private void Button_Click(object sender, RoutedEventArgs e)
        {
            if(!(ErrorMessageTextBlock.Visibility == Visibility.Visible))
            {
                if(MessageBox.Show("Game Folder is not corrected.\nPlease check Game Folder setting.\nDo you continue anyway?","Incorrect Game Folder", MessageBoxButton.YesNo) == MessageBoxResult.No)
                {
                    return;
                }
            }
            unityL10nToolCppManaged = new UnityL10nToolCppManaged(".\\", gameFolderPathTextBox.Text);
            unityL10nToolCppManaged.LoadGlobalgamemanagersFile();
            unityL10nToolCppManaged.ProcessResourceAndMonoManger();
            unityL10nToolCppManaged.LoadMonoClassDatabase();
            unityL10nToolCppManaged.LoadUnityL10nToolAPI();
            unityL10nToolCppManaged.GetOnlineResourcePairs();
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

        private void gameFolderPathTextBox_Pasting(object sender, DataObjectPastingEventArgs e)
        {
            var isText = e.SourceDataObject.GetDataPresent(DataFormats.UnicodeText, true);
            if (!isText) return;

            var text = e.SourceDataObject.GetData(DataFormats.UnicodeText) as string;

            if(UnityL10nToolCppManaged.DetermineProjectGamePath(text, GameName, MakerName))
            {
                ErrorMessageTextBlock.Visibility = Visibility.Hidden;
            } else
            {
                string tempPath = text;
                if(tempPath.Last() != '\\')
                {
                    tempPath += '\\';
                }
                tempPath += DataFolderName + "_Data\\";
                if (UnityL10nToolCppManaged.DetermineProjectGamePath(tempPath, GameName, MakerName))
                {
                    e.CancelCommand();
                    gameFolderPathTextBox.Text = tempPath;
                    ErrorMessageTextBlock.Visibility = Visibility.Hidden;
                } else
                {
                    ErrorMessageTextBlock.Visibility = Visibility.Visible;
                }
            }
        }

        private void gameFolderPathTextBox_TextChanged(object sender, TextChangedEventArgs e)
        {
            if (UnityL10nToolCppManaged.DetermineProjectGamePath(gameFolderPathTextBox.Text, GameName, MakerName))
            {
                ErrorMessageTextBlock.Visibility = Visibility.Hidden;
            }
            else
            {
                string tempPath = gameFolderPathTextBox.Text;
                if (tempPath.Last() != '\\')
                {
                    tempPath += '\\';
                }
                tempPath += DataFolderName + "_Data\\";
                if (UnityL10nToolCppManaged.DetermineProjectGamePath(tempPath, GameName, MakerName))
                {
                    gameFolderPathTextBox.Text = tempPath;
                    ErrorMessageTextBlock.Visibility = Visibility.Hidden;
                }
                else
                {
                    ErrorMessageTextBlock.Visibility = Visibility.Visible;
                }
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
                dialog.DefaultDirectory = gameFolderPathTextBox.Text;
            }

            if (dialog.ShowDialog() == CommonFileDialogResult.Ok)
            {
                string dirToProcess = Directory.Exists(dialog.FileName) ? dialog.FileName : System.IO.Path.GetDirectoryName(dialog.FileName);
                gameFolderPathTextBox.Text = dirToProcess;
            }
        }
    }
}
