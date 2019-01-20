using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Linq;
using System.Net;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Controls.Primitives;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;
using System.IO.Compression;
using UnityL10nToolCppCLI;
using Microsoft.Win32;

namespace UnityL10nToolCShop
{
    /// <summary>
    /// ProjectConfig.xaml에 대한 상호 작용 논리
    /// </summary>
    /// 
    public class TextAssetTabControlContext
    {
        public class InteractWithTextAssetContext :INotifyPropertyChanged
        {
            public ObservableCollection<TextAssetMapCLI> News { get; set; }
            public ObservableCollection<TextAssetMapCLI> Saveds { get; set; }
            private TextAssetMapCLI _SelectedItem;
            public TextAssetMapCLI SelectedItem {
                get
                {
                    return _SelectedItem;
                }
                set
                {
                    _SelectedItem = value;
                    OnPropertyChanged("SelectedItem");
                }
            }
            public InteractWithTextAssetContext(ObservableCollection<TextAssetMapCLI> News, ObservableCollection<TextAssetMapCLI> Saveds)
            {
                if (News == null)
                {
                    this.News = new ObservableCollection<TextAssetMapCLI>();

                }
                else
                {
                    this.News = News;
                }
                if (Saveds == null)
                {
                    this.Saveds = new ObservableCollection<TextAssetMapCLI>();
                }
                else
                {
                    this.Saveds = Saveds;
                }

            }
            public event PropertyChangedEventHandler PropertyChanged;
            // Create the OnPropertyChanged method to raise the event protected 
            void OnPropertyChanged(string name) {
                PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(name));
            }
        }
        public class InteractWithMonoTextAssetContext: INotifyPropertyChanged
        {
            public ObservableCollection<TextAssetMapCLI> News { get; set; }
            public ObservableCollection<TextAssetMapCLI> Saveds { get; set; }
            private TextAssetMapCLI _SelectedItem;
            public TextAssetMapCLI SelectedItem
            {
                get
                {
                    return _SelectedItem;
                }
                set
                {
                    _SelectedItem = value;
                    OnPropertyChanged("SelectedItem");
                }
            }
            public InteractWithMonoTextAssetContext(ObservableCollection<TextAssetMapCLI> News, ObservableCollection<TextAssetMapCLI> Saveds)
            {
                if(News == null)
                {
                    this.News = new ObservableCollection<TextAssetMapCLI>();

                } else
                {
                    this.News = News;
                }
                if(Saveds == null)
                {
                    this.Saveds = new ObservableCollection<TextAssetMapCLI>();
                } else
                {
                    this.Saveds = Saveds;
                }
                
            }
            public event PropertyChangedEventHandler PropertyChanged;
            // Create the OnPropertyChanged method to raise the event protected 
            void OnPropertyChanged(string name)
            {
                PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(name));
            }
        }
        public class InteractWithFileTextContext: INotifyPropertyChanged
        {
            public ObservableCollection<TextAssetMapCLI> News { get; set; }
            public ObservableCollection<TextAssetMapCLI> Saveds { get; set; }
            private TextAssetMapCLI _SelectedItem;
            public TextAssetMapCLI SelectedItem
            {
                get
                {
                    return _SelectedItem;
                }
                set
                {
                    _SelectedItem = value;
                    OnPropertyChanged("SelectedItem");
                }
            }
            public InteractWithFileTextContext(ObservableCollection<TextAssetMapCLI> News, ObservableCollection<TextAssetMapCLI> Saveds)
            {
                if (News == null)
                {
                    this.News = new ObservableCollection<TextAssetMapCLI>();

                }
                else
                {
                    this.News = News;
                }
                if (Saveds == null)
                {
                    this.Saveds = new ObservableCollection<TextAssetMapCLI>();
                }
                else
                {
                    this.Saveds = Saveds;
                }

            }
            public event PropertyChangedEventHandler PropertyChanged;
            // Create the OnPropertyChanged method to raise the event protected 
            void OnPropertyChanged(string name)
            {
                PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(name));
            }
        }
        public class DoneContext : INotifyPropertyChanged
        {
            public ObservableCollection<TextAssetMapCLI> Saveds { get; set; }
            private TextAssetMapCLI _SelectedItem;
            public TextAssetMapCLI SelectedItem
            {
                get
                {
                    return _SelectedItem;
                }
                set
                {
                    _SelectedItem = value;
                    OnPropertyChanged("SelectedItem");
                }
            }
            public DoneContext(ObservableCollection<TextAssetMapCLI> Saveds)
            {
                if (Saveds == null)
                {
                    this.Saveds = new ObservableCollection<TextAssetMapCLI>();
                }
                else
                {
                    this.Saveds = Saveds;
                }

            }
            public event PropertyChangedEventHandler PropertyChanged;
            // Create the OnPropertyChanged method to raise the event protected 
            void OnPropertyChanged(string name)
            {
                PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(name));
            }
        }
        public InteractWithTextAssetContext InteractWithTextAsset { get; set; }
        public InteractWithMonoTextAssetContext InteractWithMonoTextAsset { get; set; }
        public InteractWithFileTextContext InteractWithFileText { get; set; }
        public DoneContext Done { get; set; }
        public TextAssetTabControlContext(TextAssetMapsCLI textAssetMapsCLI)
        {
            InteractWithTextAsset = new InteractWithTextAssetContext(textAssetMapsCLI.InteractWithAssetNews, textAssetMapsCLI.InteractWithFileTextNews);
            InteractWithMonoTextAsset = new InteractWithMonoTextAssetContext(null, textAssetMapsCLI.Done);
            InteractWithFileText = new InteractWithFileTextContext(textAssetMapsCLI.InteractWithFileTextNews, textAssetMapsCLI.Done);
            Done = new DoneContext(textAssetMapsCLI.Done);
        }
    }

    public class EnumBooleanConverter : IValueConverter
    {
        #region IValueConverter Members
        public object Convert(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            string parameterString = parameter as string;
            if (parameterString == null)
                return DependencyProperty.UnsetValue;

            if (Enum.IsDefined(value.GetType(), value) == false)
                return DependencyProperty.UnsetValue;

            object parameterValue = Enum.Parse(value.GetType(), parameterString);

            return parameterValue.Equals(value);
        }

        public object ConvertBack(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            string parameterString = parameter as string;
            if (parameterString == null)
                return DependencyProperty.UnsetValue;

            return Enum.Parse(targetType, parameterString);
        }
        #endregion
    }

    public partial class ProjectConfig : Window
    {
        
        public TextAssetTabControlContext textAssetTabControlContext;
        UnityL10nToolCppManaged unityL10nToolCppManaged;
        UnityL10nToolProjectInfo unityL10NToolProjectInfo;
        BackgroundWorker LoadUnityL10nTool_BackgroundWorker;
        ProjectConfigSplash projectConfigSplash;
        Dictionary<string, FontAssetMapsCLI> pluginsSupportAssetMap;
        public FontAssetMapCLI SelectedFontAssetItem;
        List<string> interactWithAssetNames;
        List<string> interactWithFileTextNames;
        public ObservableCollection<OnlineResourcePairCLI> OnlineResourcePairCLIsGlobal { get; set; }
        public ProjectSettingsCLI ProjectSettingsCLIGlobal { get; set; }
        public OnlineUpdateCLI OnlineUpdateCLIGlobal { get; set; }
        public ObservableCollection<CopyResourcesManualPairCLI> CopyResourcesManualPairCLIsGlobal { get; set; }

        static void DownloadWebFile(string name, string path, string url)
        {
            WebClient webClient = new WebClient();
            try
            {
                webClient.DownloadFile(url, path + name);
            }
            catch
            {

            }
        }

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
            if(!System.IO.Directory.Exists(AppDomain.CurrentDomain.BaseDirectory + "temp\\"))
            {
                System.IO.Directory.CreateDirectory(AppDomain.CurrentDomain.BaseDirectory + "temp\\");
            }
            if (System.IO.File.Exists(AppDomain.CurrentDomain.BaseDirectory + "Projects\\" + unityL10NToolProjectInfo.GameName + "\\customIcon.ico"))
            {
                System.IO.File.Copy(AppDomain.CurrentDomain.BaseDirectory + "Projects\\" + unityL10NToolProjectInfo.GameName + "\\customIcon.ico",
                    AppDomain.CurrentDomain.BaseDirectory + "temp\\customIcon.ico", true);
                Uri uri = new Uri(AppDomain.CurrentDomain.BaseDirectory + "temp\\customIcon.ico", UriKind.RelativeOrAbsolute);
                PatcherIconImage.Source = BitmapFrame.Create(uri);
            }
            if (System.IO.File.Exists(AppDomain.CurrentDomain.BaseDirectory + "Projects\\" + unityL10NToolProjectInfo.GameName + "\\customSplash.png"))
            {
                System.IO.File.Copy(AppDomain.CurrentDomain.BaseDirectory + "Projects\\" + unityL10NToolProjectInfo.GameName + "\\customSplash.png",
                    AppDomain.CurrentDomain.BaseDirectory + "temp\\customSplash.png", true);
                Uri uri = new Uri(AppDomain.CurrentDomain.BaseDirectory + "Projects\\" + unityL10NToolProjectInfo.GameName + "\\customSplash.png", UriKind.RelativeOrAbsolute);
                PatcherSplashImage.Source = BitmapFrame.Create(uri);
            }
            if (System.IO.File.Exists(AppDomain.CurrentDomain.BaseDirectory + "Projects\\" + unityL10NToolProjectInfo.GameName + "\\customMain.png"))
            {
                System.IO.File.Copy(AppDomain.CurrentDomain.BaseDirectory + "Projects\\" + unityL10NToolProjectInfo.GameName + "\\customMain.png",
                    AppDomain.CurrentDomain.BaseDirectory + "temp\\customMain.png", true);
                Uri uri = new Uri(AppDomain.CurrentDomain.BaseDirectory + "Projects\\" + unityL10NToolProjectInfo.GameName + "\\customMain.png", UriKind.RelativeOrAbsolute);
                PatcherMainImage.Source = BitmapFrame.Create(uri);
            }
            LoadUnityL10nTool_BackgroundWorker = new BackgroundWorker();
            LoadUnityL10nTool_BackgroundWorker.DoWork += LoadUnityL10nTool_DoWork;
            LoadUnityL10nTool_BackgroundWorker.ProgressChanged += LoadUnityL10nTool_ProgressChanged;
            LoadUnityL10nTool_BackgroundWorker.RunWorkerCompleted += LoadUnityL10nTool_RunWorkerCompleted;
            LoadUnityL10nTool_BackgroundWorker.WorkerReportsProgress = true;
            LoadUnityL10nTool_BackgroundWorker.RunWorkerAsync();
            //LoadUnityL10nTool_DoWork(null, null);
            //LoadUnityL10nTool_RunWorkerCompleted(null, null);
        }

        private void LoadUnityL10nTool_DoWork(object sender, DoWorkEventArgs e)
        {
            string gamePath = "";
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

            if(e!=null) LoadUnityL10nTool_BackgroundWorker.ReportProgress(0, "Initialize UnityL10nTool...");
            unityL10nToolCppManaged = new UnityL10nToolCppManaged(unityL10NToolProjectInfo.JSONPath.Replace("setting.json", ""));

            if(e!=null) LoadUnityL10nTool_BackgroundWorker.ReportProgress(0, "Loading Assets...");
            unityL10nToolCppManaged.LoadGlobalgamemanagersFile(); 

            if(e!=null) LoadUnityL10nTool_BackgroundWorker.ReportProgress(0, "Loading Class Information...");
            unityL10nToolCppManaged.ProcessResourceAndMonoManger(); 

            if(e!=null) LoadUnityL10nTool_BackgroundWorker.ReportProgress(0, "Loading MonoClassDatabase...");
            unityL10nToolCppManaged.LoadMonoClassDatabase();

            if(e!=null) LoadUnityL10nTool_BackgroundWorker.ReportProgress(0, "Loading UnityL10nToolAPI...");
            unityL10nToolCppManaged.LoadUnityL10nToolAPI();

            if(e!=null) LoadUnityL10nTool_BackgroundWorker.ReportProgress(0, "Loading Font Plugins...");
            List<string> loadedFontPlugins = unityL10nToolCppManaged.LoadFontPlugins();

            if(e!=null) LoadUnityL10nTool_BackgroundWorker.ReportProgress(0, "Loading Font Plugins Support Asset List...");
            pluginsSupportAssetMap = unityL10nToolCppManaged.GetPluginsSupportAssetMap();

            if(e!=null) LoadUnityL10nTool_BackgroundWorker.ReportProgress(0, "Loading Text Plugins Support Asset List...");
            bool resultload = unityL10nToolCppManaged.LoadTextPlugins();
            TextAssetMapsCLI textAssetMapsCLILocal = unityL10nToolCppManaged.GetTextAssetMaps();
            TextAssetTabControlContext textAssetTabControlContextLocal = new TextAssetTabControlContext(textAssetMapsCLILocal);
            textAssetTabControlContext = textAssetTabControlContextLocal;

            if(e!=null) LoadUnityL10nTool_BackgroundWorker.ReportProgress(0, "Loading Text Plugins Support Asset List...");
            interactWithAssetNames = unityL10nToolCppManaged.GetInteractWithAssetPluginNames();
            //interactWithAssetNames.Insert(0, null);
            interactWithAssetNames.Insert(0, "");
            interactWithFileTextNames = unityL10nToolCppManaged.GetInteractWithFileTextPluginNames();
            interactWithFileTextNames.Insert(0, "");

            if(e!=null) LoadUnityL10nTool_BackgroundWorker.ReportProgress(0, "Loading Online Resources List...");
            OnlineResourcePairCLIsGlobal = unityL10nToolCppManaged.GetOnlineResourcePairs();

            if (e != null) LoadUnityL10nTool_BackgroundWorker.ReportProgress(0, "Loading Copy Resources by Manual List...");
            CopyResourcesManualPairCLIsGlobal = unityL10nToolCppManaged.GetCopyResourcesManualPairs();

            if(e!=null) LoadUnityL10nTool_BackgroundWorker.ReportProgress(0, "Loading Online Update...");
            OnlineUpdateCLIGlobal = unityL10nToolCppManaged.GetOnlineUpdate();

            if(e!=null) LoadUnityL10nTool_BackgroundWorker.ReportProgress(0, "Loading Project Settings...");
            unityL10nToolCppManaged.LoadProjectSettingsFromJson();
            ProjectSettingsCLIGlobal = unityL10nToolCppManaged.GetProjectSettings();
        }

        private void LoadUnityL10nTool_ProgressChanged(object sender, ProgressChangedEventArgs e)
        {
            projectConfigSplash.ProgressText = (string)e.UserState;
        }

        private void LoadUnityL10nTool_RunWorkerCompleted(object sender, RunWorkerCompletedEventArgs e)
        {
            if (e.Error != null)
            {
                MessageBox.Show(e.Error.Message);
                MessageBox.Show("Error during working. Please report issue at github");
            } else
            {
                MainGrid.Children.Remove(projectConfigSplash);
                GamePathTextBlock.Text = unityL10NToolProjectInfo.GamePath;
                FontAssetTabControl.ItemsSource = pluginsSupportAssetMap;
                TextAssetTabControl.DataContext = textAssetTabControlContext;
                PluginInteractWithAssetNamesCombobox.ItemsSource = interactWithAssetNames;
                PluginInteractWithFileTextNamesCombobox.ItemsSource = interactWithFileTextNames;
                LoadUnityL10nTool_BackgroundWorker.Dispose();
                OnlineResourcesDataGrid.ItemsSource = OnlineResourcePairCLIsGlobal;
                CopyResourcesManualDataGrid.ItemsSource = CopyResourcesManualPairCLIsGlobal;
                ProjectSettingsGrid.DataContext = ProjectSettingsCLIGlobal;
                UpdaterGrid.DataContext = OnlineUpdateCLIGlobal;
            }
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
            try
            {
                FontAssetMapCLI found = fontAssetMapsCLI.Saveds.Single(x =>
                    x.LooseEquals(fontAssetMapCLI));
                fontAssetMapsCLI.Saveds.Remove(found);
                FontAssetMapCLI clone = fontAssetMapCLI.Clone();
                fontAssetMapsCLI.Saveds.Add(clone);

            } catch (InvalidOperationException invalidOperationException)
            {
                FontAssetMapCLI clone = fontAssetMapCLI.Clone();
                fontAssetMapsCLI.Saveds.Add(clone);
            }
        }

        private void Button_Click_1(object sender, RoutedEventArgs e)
        {
            // Edit하면 command로 SelectedItem을 바꾸고,  Add 버튼을 Edit 버튼으로 바꾸기? 
            Button button = (Button)sender;
            FontAssetMapCLI fontAssetMapCLI = (FontAssetMapCLI)(button).DataContext;
            StackPanel stackPanel = (StackPanel)button.Parent;
            Grid grid = (Grid)stackPanel.Parent;
            FontAssetMapsCLI fontAssetMapsCLI = (FontAssetMapsCLI)grid.DataContext;
            Grid templateRootGrid = (Grid)grid.Parent;
            DataGrid fontAssetMapDataGrid = (DataGrid)templateRootGrid.FindName("FontAssetMapDataGrid");
            try
            {
                FontAssetMapCLI found = fontAssetMapsCLI.News.Single(x =>
                    x.LooseEquals(fontAssetMapCLI));
                int index = fontAssetMapsCLI.News.IndexOf(found);
                fontAssetMapsCLI.News.Remove(found);
                fontAssetMapsCLI.News.Insert(index, fontAssetMapCLI);
                fontAssetMapDataGrid.SelectedItem = fontAssetMapCLI;
                Button button1 = (Button)templateRootGrid.FindName("AddEditButton");
                button1.Content = "Edit";
            } catch (Exception exception1)
            {
                throw new Exception(fontAssetMapCLI + " Must be found");
            }
        }

        private void Button_Click_2(object sender, RoutedEventArgs e)
        {
            Button button = (Button)sender;
            FontAssetMapCLI fontAssetMapCLI = (FontAssetMapCLI)(button).DataContext;
            StackPanel stackPanel = (StackPanel)button.Parent;
            Grid grid = (Grid)stackPanel.Parent;
            FontAssetMapsCLI fontAssetMapsCLI = (FontAssetMapsCLI)grid.DataContext;
            try
            {
                FontAssetMapCLI found = fontAssetMapsCLI.Saveds.Single(x =>
                    x.LooseEquals(fontAssetMapCLI));
                fontAssetMapsCLI.Saveds.Remove(found);
            } catch(Exception exception1)
            {
                throw new Exception(fontAssetMapCLI + " Must be found");
            }
        }

        private void FontAssetMapDataGrid_SelectedCellsChanged(object sender, SelectedCellsChangedEventArgs e)
        {
            DataGrid dataGrid = (DataGrid)sender;
            Grid grid = (Grid)dataGrid.Parent;
            Button button = (Button)grid.FindName("AddEditButton");
            button.Content = "Add";
            button.IsEnabled = true;
        }

        private void Button_Click_3(object sender, RoutedEventArgs e)
        {
            unityL10nToolCppManaged.SetProjectSettings(ProjectSettingsCLIGlobal);
            unityL10nToolCppManaged.SetPluginsSupportAssetMap(pluginsSupportAssetMap);
            unityL10nToolCppManaged.GetProjectConfigJsonFromFontPlugin();
            unityL10nToolCppManaged.SetTextPluginConfigToJsonValue();
            unityL10nToolCppManaged.SetOnlineResourcePairs(OnlineResourcePairCLIsGlobal);
            unityL10nToolCppManaged.SetCopyResourcesManualPairs(CopyResourcesManualPairCLIsGlobal);
            unityL10nToolCppManaged.SetOnlineUpdate(OnlineUpdateCLIGlobal);
            unityL10nToolCppManaged.SaveProjectConfigJson();
        }

        private void Button_Click_4(object sender, RoutedEventArgs e)
        {
            projectConfigSplash = new ProjectConfigSplash();
            projectConfigSplash.SetValue(Grid.RowSpanProperty, 2);
            projectConfigSplash.SetValue(Grid.ColumnSpanProperty, 2);

            MainGrid.Children.Add(projectConfigSplash);
            LoadUnityL10nTool_BackgroundWorker = new BackgroundWorker();
            LoadUnityL10nTool_BackgroundWorker.DoWork += (workerSender, workerE) =>
            {
                LoadUnityL10nTool_BackgroundWorker.ReportProgress(0, "Cleaning for building");
                string projectFolderName = unityL10NToolProjectInfo.JSONPath.Replace("setting.json", "");
                Button_Click_3(null, null);
                if (ProjectSettingsCLIGlobal.RemoveBuildFolderBeforeBuild)
                {
                    System.IO.DirectoryInfo di = new System.IO.DirectoryInfo(projectFolderName + "Build\\");
                    // Delete this dir and all subdirs.
                    try
                    {
                        di.Delete(true);
                    }
                    catch (System.IO.IOException ioException)
                    {
                        Console.WriteLine(ioException.Message);
                    }
                }
                LoadUnityL10nTool_BackgroundWorker.ReportProgress(0, "Building");
                unityL10nToolCppManaged.BuildProject(projectFolderName + "Build\\");
                string binaryName;
                if (ProjectSettingsCLIGlobal.ZipFileName != "")
                {
                    binaryName = ProjectSettingsCLIGlobal.ZipFileName;
                }
                else
                {
                    binaryName = unityL10NToolProjectInfo.GameName + " Patcher";
                }
                System.IO.File.Move(
                    projectFolderName + "Build\\UnityL10nToolPatcherCShop.exe",
                    projectFolderName + "Build\\" + binaryName + ".exe");
                if (ProjectSettingsCLIGlobal.ZipBuildFolderAfterBuild)
                {
                    LoadUnityL10nTool_BackgroundWorker.ReportProgress(0, "Zipping built result");
                    string ZipPath;
                    string zipFileNamePrefix;
                    if (ProjectSettingsCLIGlobal.ZipFileName != "")
                    {
                        zipFileNamePrefix = ProjectSettingsCLIGlobal.ZipFileName;
                    }
                    else
                    {
                        zipFileNamePrefix = unityL10NToolProjectInfo.GameName;
                    }
                    if (OnlineUpdateCLIGlobal.currentVersion != "")
                    {
                        if (ProjectSettingsCLIGlobal.CreateCurrentVersionTxtFileAfterBuild)
                        {
                            System.IO.File.WriteAllText(projectFolderName + "CurrentVersion.txt", OnlineUpdateCLIGlobal.currentVersion);
                        }
                        if (ProjectSettingsCLIGlobal.IsZipFileNameConatinVersion)
                        {
                            ZipPath = projectFolderName + zipFileNamePrefix + " " + OnlineUpdateCLIGlobal.currentVersion + ".zip";
                        }
                        else
                        {
                            ZipPath = projectFolderName + zipFileNamePrefix + ".zip";
                        }
                    }
                    else
                    {
                        ZipPath = projectFolderName + zipFileNamePrefix + ".zip";
                    }
                    if (System.IO.File.Exists(ZipPath))
                    {
                        System.IO.File.Delete(ZipPath);
                    }
                    ZipFile.CreateFromDirectory(projectFolderName + "Build\\", ZipPath);
                }
            };
            LoadUnityL10nTool_BackgroundWorker.ProgressChanged += (workerSender, workerE)=>
            {
                projectConfigSplash.ProgressText = (string)workerE.UserState;
            };
            LoadUnityL10nTool_BackgroundWorker.RunWorkerCompleted += (workerSender, workerE)=>
            {
                MainGrid.Children.Remove(projectConfigSplash);
            };
            LoadUnityL10nTool_BackgroundWorker.WorkerReportsProgress = true;
            LoadUnityL10nTool_BackgroundWorker.RunWorkerAsync();
        }

        private void CustomProperties_DataContextChanged(object sender, DependencyPropertyChangedEventArgs e)
        {

        }
        
        private void Grid_DataContextChanged(object sender, DependencyPropertyChangedEventArgs e)
        {
            if (e.NewValue != null && e.OldValue != null)
            {
                TextAssetMapCLI NewPair = (TextAssetMapCLI)e.NewValue;
                TextAssetMapCLI OldPair = (TextAssetMapCLI)e.OldValue;
                if (NewPair != null && OldPair != null &&
                    NewPair.assetName == OldPair.assetName &&
                    NewPair.assetsName == OldPair.assetsName &&
                    NewPair.containerPath == OldPair.containerPath)
                {
                    return;
                }
            }
            if (e.NewValue != null)
            {
                TextAssetMapCLI newTAM = (TextAssetMapCLI)e.NewValue;
                if (!textAssetTabControlContext.InteractWithTextAsset.News.Contains(newTAM))
                {
                    return;
                }
                TextAssetMapCLI textAssetMapCLILocal = unityL10nToolCppManaged.GetOriginalLanguagePairDics(newTAM);
                TextAssetMapCLI oldTAM = textAssetTabControlContext.InteractWithTextAsset.SelectedItem;
                int i = textAssetTabControlContext.InteractWithTextAsset.News.IndexOf(oldTAM);
                // https://stackoverflow.com/questions/9829969/comboboxs-selecteditem-is-unexpectedly-set-to-null-before-actual-value
                textAssetTabControlContext.InteractWithTextAsset.News.Insert(i, textAssetMapCLILocal);
                textAssetTabControlContext.InteractWithTextAsset.SelectedItem = textAssetMapCLILocal;
                textAssetTabControlContext.InteractWithTextAsset.News.RemoveAt(i + 1);
                //InteractWithTextAssetDataGridNews.SelectedIndex = i;
                //SelectRowByIndex(InteractWithTextAssetDataGridNews, i);
            }
        }

#region SelectRowByIndex https://social.technet.microsoft.com/wiki/contents/articles/21202.wpf-programmatically-selecting-and-focusing-a-row-or-cell-in-a-datagrid.aspx
        public static void SelectRowByIndex(DataGrid dataGrid, int rowIndex)
        {
            if (!dataGrid.SelectionUnit.Equals(DataGridSelectionUnit.FullRow))
                throw new ArgumentException("The SelectionUnit of the DataGrid must be set to FullRow.");

            if (rowIndex < 0 || rowIndex > (dataGrid.Items.Count - 1))
                throw new ArgumentException(string.Format("{0} is an invalid row index.", rowIndex));

            //dataGrid.SelectedItems.Clear();
            ///* set the SelectedItem property */
            //object item = dataGrid.Items[rowIndex]; // = Product X
            //dataGrid.SelectedItem = item;

            DataGridRow row = dataGrid.ItemContainerGenerator.ContainerFromIndex(rowIndex) as DataGridRow;
            if (row != null)
            {
                DataGridCell cell = GetCell(dataGrid, row, 0);
                if (cell != null)
                    cell.Focus();
            }
            //TODO: Retrieve and focus a DataGridCell object
        }

        public static DataGridCell GetCell(DataGrid dataGrid, DataGridRow rowContainer, int column)
        {
            if (rowContainer != null)
            {
                DataGridCellsPresenter presenter = FindVisualChild<DataGridCellsPresenter>(rowContainer);
                if (presenter == null)
                {
                    /* if the row has been virtualized away, call its ApplyTemplate() method
                     * to build its visual tree in order for the DataGridCellsPresenter
                     * and the DataGridCells to be created */
                    rowContainer.ApplyTemplate();
                    presenter = FindVisualChild<DataGridCellsPresenter>(rowContainer);
                }
                if (presenter != null)
                {
                    DataGridCell cell = presenter.ItemContainerGenerator.ContainerFromIndex(column) as DataGridCell;
                    if (cell == null)
                    {
                        /* bring the column into view
                         * in case it has been virtualized away */
                        dataGrid.ScrollIntoView(rowContainer, dataGrid.Columns[column]);
                        cell = presenter.ItemContainerGenerator.ContainerFromIndex(column) as DataGridCell;
                    }
                    return cell;
                }
            }
            return null;
        }

        public static T FindVisualChild<T>(DependencyObject obj) where T : DependencyObject
        {
            for (int i = 0; i < VisualTreeHelper.GetChildrenCount(obj); i++)
            {
                DependencyObject child = VisualTreeHelper.GetChild(obj, i);
                if (child != null && child is T)
                    return (T)child;
                else
                {
                    T childOfChild = FindVisualChild<T>(child);
                    if (childOfChild != null)
                        return childOfChild;
                }
            }
            return null;
        }
        #endregion

        private void PluginInteractWithAssetNamesCombobox_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            if(e.AddedItems.Count != 0 && e.RemovedItems.Count != 0 && (string)e.AddedItems[0] == (string)e.RemovedItems[0])
            {
                return;
            }
            TextAssetMapCLI newTAM = textAssetTabControlContext.InteractWithTextAsset.SelectedItem;
            if (!textAssetTabControlContext.InteractWithTextAsset.News.Contains(newTAM))
            {
                return;
            }
            if(e.AddedItems.Count != 0)
            {
                newTAM.InteractWithAssetPluginName = (string)e.AddedItems[0];
            }
            TextAssetMapCLI textAssetMapCLILocal = unityL10nToolCppManaged.GetOriginalLanguagePairDics(newTAM);
            int i = textAssetTabControlContext.InteractWithTextAsset.News.IndexOf(newTAM);
            textAssetTabControlContext.InteractWithTextAsset.News.Insert(i, textAssetMapCLILocal);
            textAssetTabControlContext.InteractWithTextAsset.SelectedItem = textAssetMapCLILocal;
            textAssetTabControlContext.InteractWithTextAsset.News.RemoveAt(i + 1);
            //InteractWithTextAssetDataGridNews.SelectedIndex = i;
            //SelectRowByIndex(InteractWithTextAssetDataGridNews, i);
        }

        private void LanguagePairDicsCombobox_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            if (sender is ComboBox comboBox)
            {
                if (e.AddedItems.Count == 0 && e.RemovedItems.Count == 1)
                {
                    KeyValuePair<string, LanguagePairDicCLI> prevPair = (KeyValuePair<string, LanguagePairDicCLI>)e.RemovedItems[0];
                    TextAssetMapCLI textAssetMapCLI = textAssetTabControlContext.InteractWithTextAsset.SelectedItem;
                    if (textAssetMapCLI.languagePairDics.Count != 0)
                    {
                        KeyValuePair<string, LanguagePairDicCLI> found = textAssetMapCLI.languagePairDics.FirstOrDefault(x => x.Key == prevPair.Key);
                        if (found.Key != null && found.Value != null)
                        {
                            comboBox.SelectedItem = found;
                        }
                        else
                        {
                            comboBox.SelectedItem = textAssetMapCLI.languagePairDics.First();
                        }
                    }
                }
            }
        }

        private void interactWithTextAssetProperties_OptionChanged(object sender, DependencyPropertyChangedEventArgs e)
        {
            TextAssetMapCLI NewPair = textAssetTabControlContext.InteractWithTextAsset.SelectedItem;
            TextAssetMapCLI textAssetMapCLILocal = unityL10nToolCppManaged.GetOriginalLanguagePairDics(NewPair);
            int i = textAssetTabControlContext.InteractWithTextAsset.News.IndexOf(NewPair);
            textAssetTabControlContext.InteractWithTextAsset.News.Insert(i, textAssetMapCLILocal);
            textAssetTabControlContext.InteractWithTextAsset.SelectedItem = textAssetMapCLILocal;
            textAssetTabControlContext.InteractWithTextAsset.News.RemoveAt(i + 1);
        }

        private void LanguagePairDicsNew_Click(object sender, RoutedEventArgs e)
        {
            TextAssetMapCLI selectedItem = textAssetTabControlContext.InteractWithTextAsset.SelectedItem;
            if(selectedItem != null)
            {
                Dictionary<string, LanguagePairDicCLI> languagePairDics = selectedItem.languagePairDics;
                AddNewDics(languagePairDics, languagePairDics.Count);
                TextAssetMapCLI textAssetMapCLILocal = unityL10nToolCppManaged.GetOriginalLanguagePairDics(selectedItem);
                int i = textAssetTabControlContext.InteractWithTextAsset.News.IndexOf(selectedItem);
                textAssetTabControlContext.InteractWithTextAsset.News.Insert(i, textAssetMapCLILocal);
                textAssetTabControlContext.InteractWithTextAsset.SelectedItem = textAssetMapCLILocal;
                textAssetTabControlContext.InteractWithTextAsset.News.RemoveAt(i + 1);
            }
        }

        private void AddNewDics(Dictionary<string, LanguagePairDicCLI> languagePairDics, int key)
        {
            if (languagePairDics.ContainsKey(key.ToString()))
            {
                AddNewDics(languagePairDics, key + 1);
            } else
            {
                languagePairDics[key.ToString()] = new LanguagePairDicCLI();
            }
        }

        private void LanguagePairDicsRemove_Click(object sender, RoutedEventArgs e)
        {
            TextAssetMapCLI selectedItem = textAssetTabControlContext.InteractWithTextAsset.SelectedItem;
            if(selectedItem != null)
            {
                Dictionary<string, LanguagePairDicCLI> languagePairDics = selectedItem.languagePairDics;
                KeyValuePair<string, LanguagePairDicCLI> LanguagePairDicCLISelected = (KeyValuePair<string, LanguagePairDicCLI>)LanguagePairDicsCombobox.SelectedItem;
                languagePairDics.Remove(LanguagePairDicCLISelected.Key);
                TextAssetMapCLI textAssetMapCLILocal = unityL10nToolCppManaged.GetOriginalLanguagePairDics(selectedItem);
                int i = textAssetTabControlContext.InteractWithTextAsset.News.IndexOf(selectedItem);
                textAssetTabControlContext.InteractWithTextAsset.News.Insert(i, textAssetMapCLILocal);
                textAssetTabControlContext.InteractWithTextAsset.SelectedItem = textAssetMapCLILocal;
                textAssetTabControlContext.InteractWithTextAsset.News.RemoveAt(i + 1);
            }
        }

        private void AddToInteractWithFileAssetEditButton_Click(object sender, RoutedEventArgs e)
        {
            if(sender is Button button)
            {
                if((string)button.Content == "Add")
                {
                    TextAssetMapCLI selectedItem = textAssetTabControlContext.InteractWithTextAsset.SelectedItem;
                    if(unityL10nToolCppManaged.SetTextAssetMaps(selectedItem, TextAssetMapCLI.ToWhere.ToInteractWithFileText)){
                        textAssetTabControlContext.InteractWithTextAsset.Saveds.Add(selectedItem);
                        textAssetTabControlContext.InteractWithTextAsset.News.Remove(selectedItem);
                        textAssetTabControlContext.InteractWithTextAsset.SelectedItem = null;
                    }
                    
                }
                else
                {
                    TextAssetMapCLI selectedItem = textAssetTabControlContext.InteractWithTextAsset.SelectedItem;
                    if(unityL10nToolCppManaged.SetTextAssetMaps(selectedItem, TextAssetMapCLI.ToWhere.ToInteractWithAsset))
                    {
                        interactWithTextAssetProperties.IsReadOnly = true;
                        PluginInteractWithAssetNamesCombobox.IsEnabled = false;
                        textAssetTabControlContext.InteractWithTextAsset.News.Add(selectedItem);
                        textAssetTabControlContext.InteractWithTextAsset.Saveds.Remove(selectedItem);
                        textAssetTabControlContext.InteractWithTextAsset.SelectedItem = selectedItem;
                        interactWithTextAssetProperties.Refresh();
                    }
                }
            }
        }

        private void InteractWithTextAssetDataGridNews_GotFocus(object sender, RoutedEventArgs e)
        {
            interactWithTextAssetProperties.IsReadOnly = false;
            PluginInteractWithAssetNamesCombobox.IsEnabled = true;
            AddToInteractWithFileAssetEditButton.Content = "Add";
            AddToInteractWithFileAssetEditButton.IsEnabled = true;
        }

        private void InteractWithTextAssetDataGridSaveds_GotFocus(object sender, RoutedEventArgs e)
        {
            if (InteractWithTextAssetDataGridSaveds.SelectedItem is TextAssetMapCLI textAssetMapCLI)
            {
                interactWithTextAssetProperties.IsReadOnly = true;
                PluginInteractWithAssetNamesCombobox.IsEnabled = false;
                int i = textAssetTabControlContext.InteractWithTextAsset.Saveds.IndexOf(textAssetMapCLI);
                TextAssetMapCLI textAssetMapCLILocal = unityL10nToolCppManaged.GetOriginalLanguagePairDics(textAssetMapCLI);
                textAssetTabControlContext.InteractWithTextAsset.Saveds.Insert(i, textAssetMapCLILocal);
                textAssetTabControlContext.InteractWithTextAsset.SelectedItem = textAssetMapCLILocal;
                textAssetTabControlContext.InteractWithTextAsset.Saveds.RemoveAt(i + 1);
                AddToInteractWithFileAssetEditButton.Content = "Edit";
                if(textAssetMapCLI.languagePairDics.Count!=0 )
                {
                    LanguagePairDicsCombobox.SelectedIndex = 0;
                }
            }
        }

        #region InteractWithFileText

        private void InteractWithFileTextDataGridNews_GotFocus(object sender, RoutedEventArgs e)
        {
            interactWithFileTextProperties.IsReadOnly = false;
            PluginInteractWithFileTextNamesCombobox.IsEnabled = true;
            AddToDoneEditButton.Content = "Add";
            AddToDoneEditButton.IsEnabled = true;
        }

        private void InteractWithFileTextDataGridSaveds_GotFocus(object sender, RoutedEventArgs e)
        {
            if (InteractWithFileTextDataGridSaveds.SelectedItem is TextAssetMapCLI textAssetMapCLI)
            {
                interactWithFileTextProperties.IsReadOnly = true;
                PluginInteractWithFileTextNamesCombobox.IsEnabled = false;
                int i = textAssetTabControlContext.InteractWithFileText.Saveds.IndexOf(textAssetMapCLI);
                TextAssetMapCLI textAssetMapCLILocal = unityL10nToolCppManaged.GetUpdateFileText(textAssetMapCLI);
                textAssetTabControlContext.InteractWithFileText.Saveds.Insert(i, textAssetMapCLILocal);
                textAssetTabControlContext.InteractWithFileText.SelectedItem = textAssetMapCLILocal;
                textAssetTabControlContext.InteractWithFileText.Saveds.RemoveAt(i + 1);
                AddToDoneEditButton.Content = "Edit";
                if (textAssetMapCLI.languagePairDics.Count != 0)
                {
                    LanguagePairDicsCombobox.SelectedIndex = 0;
                }
            }
        }

        private void InteractWithFileTextSelectedItemGrid_DataContextChanged(object sender, DependencyPropertyChangedEventArgs e)
        {
            if (e.NewValue != null && e.OldValue != null)
            {
                TextAssetMapCLI NewPair = (TextAssetMapCLI)e.NewValue;
                TextAssetMapCLI OldPair = (TextAssetMapCLI)e.OldValue;
                if (NewPair != null && OldPair != null &&
                    NewPair.assetName == OldPair.assetName &&
                    NewPair.assetsName == OldPair.assetsName &&
                    NewPair.containerPath == OldPair.containerPath)
                {
                    return;
                }
            }
            if (e.NewValue != null)
            {
                TextAssetMapCLI newTAM = (TextAssetMapCLI)e.NewValue;
                if (!textAssetTabControlContext.InteractWithFileText.News.Contains(newTAM))
                {
                    return;
                }
                TextAssetMapCLI textAssetMapCLILocal = unityL10nToolCppManaged.GetUpdateFileText(newTAM);
                TextAssetMapCLI oldTAM = textAssetTabControlContext.InteractWithFileText.SelectedItem;
                int i = textAssetTabControlContext.InteractWithFileText.News.IndexOf(oldTAM);
                // https://stackoverflow.com/questions/9829969/comboboxs-selecteditem-is-unexpectedly-set-to-null-before-actual-value
                textAssetTabControlContext.InteractWithFileText.News.Insert(i, textAssetMapCLILocal);
                textAssetTabControlContext.InteractWithFileText.SelectedItem = textAssetMapCLILocal;
                textAssetTabControlContext.InteractWithFileText.News.Remove(oldTAM);
                //InteractWithTextAssetDataGridNews.SelectedIndex = i;
                //SelectRowByIndex(InteractWithTextAssetDataGridNews, i);
            }
        }

        private void PluginInteractWithFileTextNamesCombobox_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            if (e.AddedItems.Count != 0 && e.RemovedItems.Count != 0 && (string)e.AddedItems[0] == (string)e.RemovedItems[0])
            {
                return;
            }
            TextAssetMapCLI newTAM = textAssetTabControlContext.InteractWithFileText.SelectedItem;
            if (!textAssetTabControlContext.InteractWithFileText.News.Contains(newTAM))
            {
                return;
            }
            if (e.AddedItems.Count != 0)
            {
                newTAM.InteractWithFileTextPluginName = (string)e.AddedItems[0];
            }
            TextAssetMapCLI textAssetMapCLILocal = unityL10nToolCppManaged.GetUpdateFileText(newTAM);
            int i = textAssetTabControlContext.InteractWithFileText.News.IndexOf(newTAM);
            textAssetTabControlContext.InteractWithFileText.News.Insert(i, textAssetMapCLILocal);
            textAssetTabControlContext.InteractWithFileText.SelectedItem = textAssetMapCLILocal;
            textAssetTabControlContext.InteractWithFileText.News.RemoveAt(i + 1);
            //InteractWithTextAssetDataGridNews.SelectedIndex = i;
            //SelectRowByIndex(InteractWithTextAssetDataGridNews, i);
        }

        private void LanguagePairDicsComboboxOfWithFileText_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            if (sender is ComboBox comboBox)
            {
                if (e.AddedItems.Count == 0 && e.RemovedItems.Count == 1)
                {
                    KeyValuePair<string, LanguagePairDicCLI> prevPair = (KeyValuePair<string, LanguagePairDicCLI>)e.RemovedItems[0];
                    TextAssetMapCLI textAssetMapCLI = textAssetTabControlContext.InteractWithFileText.SelectedItem;
                    if (textAssetMapCLI.languagePairDics.Count != 0)
                    {
                        KeyValuePair<string, LanguagePairDicCLI> found = textAssetMapCLI.languagePairDics.FirstOrDefault(x => x.Key == prevPair.Key);
                        if (found.Key != null && found.Value != null)
                        {
                            comboBox.SelectedItem = found;
                        }
                        else
                        {
                            comboBox.SelectedItem = textAssetMapCLI.languagePairDics.First();
                        }
                    }
                }
            }
        }

        private void LanguagePairDicsNewOfWithFileText_Click(object sender, RoutedEventArgs e)
        {
            TextAssetMapCLI selectedItem = textAssetTabControlContext.InteractWithFileText.SelectedItem;
            if (selectedItem != null)
            {
                Dictionary<string, LanguagePairDicCLI> languagePairDics = selectedItem.languagePairDics;
                AddNewDics(languagePairDics, languagePairDics.Count);
                TextAssetMapCLI textAssetMapCLILocal = unityL10nToolCppManaged.GetUpdateFileText(selectedItem);
                int i = textAssetTabControlContext.InteractWithFileText.News.IndexOf(selectedItem);
                textAssetTabControlContext.InteractWithFileText.News.Insert(i, textAssetMapCLILocal);
                textAssetTabControlContext.InteractWithFileText.SelectedItem = textAssetMapCLILocal;
                textAssetTabControlContext.InteractWithFileText.News.RemoveAt(i + 1);
            }
        }

        private void LanguagePairDicsRemoveOfWithFileText_Click(object sender, RoutedEventArgs e)
        {
            TextAssetMapCLI selectedItem = textAssetTabControlContext.InteractWithFileText.SelectedItem;
            if (selectedItem != null)
            {
                Dictionary<string, LanguagePairDicCLI> languagePairDics = selectedItem.languagePairDics;
                KeyValuePair<string, LanguagePairDicCLI> LanguagePairDicCLISelected = (KeyValuePair<string, LanguagePairDicCLI>)LanguagePairDicsCombobox.SelectedItem;
                languagePairDics.Remove(LanguagePairDicCLISelected.Key);
                TextAssetMapCLI textAssetMapCLILocal = unityL10nToolCppManaged.GetUpdateFileText(selectedItem);
                int i = textAssetTabControlContext.InteractWithFileText.News.IndexOf(selectedItem);
                textAssetTabControlContext.InteractWithFileText.News.Insert(i, textAssetMapCLILocal);
                textAssetTabControlContext.InteractWithFileText.SelectedItem = textAssetMapCLILocal;
                textAssetTabControlContext.InteractWithFileText.News.RemoveAt(i + 1);
            }
        }

        private void interactWithFileTextProperties_OptionChanged(object sender, DependencyPropertyChangedEventArgs e)
        {
            TextAssetMapCLI NewPair = textAssetTabControlContext.InteractWithFileText.SelectedItem;
            TextAssetMapCLI textAssetMapCLILocal = unityL10nToolCppManaged.GetUpdateFileText(NewPair);
            int i = textAssetTabControlContext.InteractWithFileText.News.IndexOf(NewPair);
            textAssetTabControlContext.InteractWithFileText.News.Insert(i, textAssetMapCLILocal);
            textAssetTabControlContext.InteractWithFileText.SelectedItem = textAssetMapCLILocal;
            textAssetTabControlContext.InteractWithFileText.News.RemoveAt(i + 1);
        }

        private void AddToDoneEditButton_Click(object sender, RoutedEventArgs e)
        {
            if (sender is Button button)
            {
                if ((string)button.Content == "Add")
                {
                    TextAssetMapCLI selectedItem = textAssetTabControlContext.InteractWithFileText.SelectedItem;
                    if (unityL10nToolCppManaged.SetTextAssetMaps(selectedItem, TextAssetMapCLI.ToWhere.ToDone))
                    {
                        textAssetTabControlContext.InteractWithFileText.Saveds.Add(selectedItem);
                        textAssetTabControlContext.InteractWithFileText.News.Remove(selectedItem);
                        textAssetTabControlContext.InteractWithFileText.SelectedItem = null;
                    }

                }
                else
                {
                    TextAssetMapCLI selectedItem = textAssetTabControlContext.InteractWithFileText.SelectedItem;
                    if (unityL10nToolCppManaged.SetTextAssetMaps(selectedItem, TextAssetMapCLI.ToWhere.ToInteractWithFileText))
                    {
                        interactWithFileTextProperties.IsReadOnly = true;
                        PluginInteractWithFileTextNamesCombobox.IsEnabled = false;
                        textAssetTabControlContext.InteractWithFileText.News.Add(selectedItem);
                        textAssetTabControlContext.InteractWithFileText.Saveds.Remove(selectedItem);
                        textAssetTabControlContext.InteractWithFileText.SelectedItem = selectedItem;
                        interactWithFileTextProperties.Refresh();
                    }
                }
            }
        }


        #endregion

        private void UpdateTestGetOriginalDicButton_Click(object sender, RoutedEventArgs e)
        {
            TextAssetMapCLI selectedItem = textAssetTabControlContext.Done.SelectedItem;
            TextAssetMapCLI textAssetMapCLIResult = unityL10nToolCppManaged.GetOriginalLanguagePairDics(selectedItem, true);
            int i= textAssetTabControlContext.Done.Saveds.IndexOf(selectedItem);
            textAssetTabControlContext.Done.Saveds.Insert(i, textAssetMapCLIResult);
            textAssetTabControlContext.Done.SelectedItem = textAssetMapCLIResult;
            textAssetTabControlContext.Done.Saveds.Remove(selectedItem);
        }

        private void UpdateTestLoadTranslatedFileTextButton_Click(object sender, RoutedEventArgs e)
        {
            TextAssetMapCLI selectedItem = textAssetTabControlContext.Done.SelectedItem;
            TextAssetMapCLI textAssetMapCLIResult = unityL10nToolCppManaged.LoadTranslatedFileTextFromTempAndResourceFolder(selectedItem);
            int i = textAssetTabControlContext.Done.Saveds.IndexOf(selectedItem);
            textAssetTabControlContext.Done.Saveds.Insert(i, textAssetMapCLIResult);
            textAssetTabControlContext.Done.SelectedItem = textAssetMapCLIResult;
            textAssetTabControlContext.Done.Saveds.Remove(selectedItem);
        }

        private void UpdateTestGetTranslatedDicsButton_Click(object sender, RoutedEventArgs e)
        {
            TextAssetMapCLI selectedItem = textAssetTabControlContext.Done.SelectedItem;
            TextAssetMapCLI textAssetMapCLIResult = unityL10nToolCppManaged.GetTranslatedLanguagePairDics(selectedItem, true);
            int i = textAssetTabControlContext.Done.Saveds.IndexOf(selectedItem);
            textAssetTabControlContext.Done.Saveds.Insert(i, textAssetMapCLIResult);
            textAssetTabControlContext.Done.SelectedItem = textAssetMapCLIResult;
            textAssetTabControlContext.Done.Saveds.Remove(selectedItem);
        }

        private void UpdateTestGetUpdatedFileText_Click(object sender, RoutedEventArgs e)
        {
            TextAssetMapCLI selectedItem = textAssetTabControlContext.Done.SelectedItem;
            TextAssetMapCLI textAssetMapCLIResult = unityL10nToolCppManaged.GetUpdateFileText(selectedItem, true);
            int i = textAssetTabControlContext.Done.Saveds.IndexOf(selectedItem);
            textAssetTabControlContext.Done.Saveds.Insert(i, textAssetMapCLIResult);
            textAssetTabControlContext.Done.SelectedItem = textAssetMapCLIResult;
            textAssetTabControlContext.Done.Saveds.Remove(selectedItem);
        }

        private void UpdateTestSaveFileTextToTempFolder_Click(object sender, RoutedEventArgs e)
        {
            TextAssetMapCLI selectedItem = textAssetTabControlContext.Done.SelectedItem;
            bool result = unityL10nToolCppManaged.SaveUpdateFileToTempFolder(selectedItem);
        }

        private void UpdateTestGetTranslatedText_Click(object sender, RoutedEventArgs e)
        {
            TextAssetMapCLI selectedItem = textAssetTabControlContext.Done.SelectedItem;
            TextAssetMapCLI textAssetMapCLIResult = unityL10nToolCppManaged.GetTranslatedText(selectedItem, true);
            int i = textAssetTabControlContext.Done.Saveds.IndexOf(selectedItem);
            textAssetTabControlContext.Done.Saveds.Insert(i, textAssetMapCLIResult);
            textAssetTabControlContext.Done.SelectedItem = textAssetMapCLIResult;
            textAssetTabControlContext.Done.Saveds.Remove(selectedItem);
        }

        private void UpdateTestDownloadOnlineResources_Click(object sender, RoutedEventArgs e)
        {
            unityL10nToolCppManaged.DownloadResourcesFromInternetToTempFolder();
        }

        private void UseContainerPathCheckChanged(object sender, RoutedEventArgs e)
        {
            TextAssetMapCLI selectedItem = textAssetTabControlContext.InteractWithTextAsset.SelectedItem;
            unityL10nToolCppManaged.SetTextAssetMaps(selectedItem, TextAssetMapCLI.ToWhere.None);
        }

        private string StripFileName(String name)
        {
            foreach (var ch in System.IO.Path.GetInvalidFileNameChars())
            {
                return System.Text.RegularExpressions.Regex.Replace(name, "[\\\\/:*?\"<>|]", "");
            }
            return name;

        }

        private void PatcherIconImage_Click(object sender, RoutedEventArgs e)
        {
            OpenFileDialog openFileDialog = new OpenFileDialog();
            openFileDialog.Filter = "Icon resource (*.ico)|*.ico";
            if(openFileDialog.ShowDialog() == true)
            {
                string iconFilePath = openFileDialog.FileName;
                if(System.IO.File.Exists(iconFilePath))
                {
                    System.IO.File.Copy(iconFilePath, AppDomain.CurrentDomain.BaseDirectory + "Projects\\" + StripFileName(unityL10NToolProjectInfo.GameName) + "\\customIcon.ico", true);
                    System.IO.File.Copy(iconFilePath, AppDomain.CurrentDomain.BaseDirectory + "temp\\customIcon.ico", true);
                    Uri uri = new Uri(AppDomain.CurrentDomain.BaseDirectory + "temp\\customIcon.ico", UriKind.RelativeOrAbsolute);
                    PatcherIconImage.Source = BitmapFrame.Create(uri);
                }
            }
        }

        private void PatcherSplashImage_Click(object sender, RoutedEventArgs e)
        {
            OpenFileDialog openFileDialog = new OpenFileDialog();
            openFileDialog.Filter = "png image (*.png)|*.png";
            if (openFileDialog.ShowDialog() == true)
            {
                string pngFilePath = openFileDialog.FileName;
                if (System.IO.File.Exists(pngFilePath))
                {
                    System.IO.File.Copy(pngFilePath, AppDomain.CurrentDomain.BaseDirectory + "Projects\\" + StripFileName(unityL10NToolProjectInfo.GameName) + "\\customSplash.png", true);
                    System.IO.File.Copy(pngFilePath, AppDomain.CurrentDomain.BaseDirectory + "temp\\customSplash.png", true);
                    Uri uri = new Uri(AppDomain.CurrentDomain.BaseDirectory + "temp\\customSplash.png", UriKind.RelativeOrAbsolute);
                    PatcherSplashImage.Source = BitmapFrame.Create(uri);
                }
            }
        }

        private void PatcherMainImage_Click(object sender, RoutedEventArgs e)
        {
            OpenFileDialog openFileDialog = new OpenFileDialog();
            openFileDialog.Filter = "png image (*.png)|*.png";
            if (openFileDialog.ShowDialog() == true)
            {
                string pngFilePath = openFileDialog.FileName;
                if (System.IO.File.Exists(pngFilePath))
                {
                    System.IO.File.Copy(pngFilePath, AppDomain.CurrentDomain.BaseDirectory + "Projects\\" + StripFileName(unityL10NToolProjectInfo.GameName) + "\\customMain.png", true);
                    System.IO.File.Copy(pngFilePath, AppDomain.CurrentDomain.BaseDirectory + "temp\\customMain.png", true);
                    Uri uri = new Uri(AppDomain.CurrentDomain.BaseDirectory + "temp\\customMain.png", UriKind.RelativeOrAbsolute);
                    PatcherMainImage.Source = BitmapFrame.Create(uri);
                }
            }
        }

        private void LoadFromExternalFileButton_Click(object sender, RoutedEventArgs e)
        {
            OpenFileDialog openFileDialog = new OpenFileDialog();
            if(openFileDialog.ShowDialog() == true)
            {
                if(System.IO.File.Exists(openFileDialog.FileName))
                {
                    TextAssetMapCLI textAssetMapCLI = unityL10nToolCppManaged.GetTextAssetMapFromExternalFile(openFileDialog.FileName);
                    textAssetTabControlContext.InteractWithTextAsset.News.Add(textAssetMapCLI);
                }
            }
        }
    }
}
