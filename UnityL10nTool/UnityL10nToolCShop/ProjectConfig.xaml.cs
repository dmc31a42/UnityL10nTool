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
    /// 
    public class TextAssetTabControlContext
    {
        public class InteractWithTextAssetContext :INotifyPropertyChanged
        {
            public ObservableCollection<KeyValuePair<string,TextAssetMapCLI>> News { get; set; }
            public ObservableCollection<KeyValuePair<string,TextAssetMapCLI>> Saveds { get; set; }
            private KeyValuePair<string, TextAssetMapCLI> _SelectedItem;
            public KeyValuePair<string, TextAssetMapCLI> SelectedItem {
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
            public InteractWithTextAssetContext()
            {
                News = new ObservableCollection<KeyValuePair<string, TextAssetMapCLI>>();
                Saveds = new ObservableCollection<KeyValuePair<string, TextAssetMapCLI>>();
            }

            public event PropertyChangedEventHandler PropertyChanged;
            // Create the OnPropertyChanged method to raise the event protected 
            void OnPropertyChanged(string name) {
                PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(name));
            }
        }
        public class InteractWithMonoTextAssetContext
        {
            public ObservableCollection<TextAssetMapCLI> News { get; set; }
            public ObservableCollection<TextAssetMapCLI> Saveds { get; set; }
            public InteractWithMonoTextAssetContext()
            {
                News = new ObservableCollection<TextAssetMapCLI>();
                Saveds = new ObservableCollection<TextAssetMapCLI>();
            }
        }
        public class InteractWithFileTextContext
        {
            public ObservableCollection<TextAssetMapCLI> News { get; set; }
            public ObservableCollection<TextAssetMapCLI> Saveds { get; set; }
            public InteractWithFileTextContext()
            {
                News = new ObservableCollection<TextAssetMapCLI>();
                Saveds = new ObservableCollection<TextAssetMapCLI>();
            }
        }
        public InteractWithTextAssetContext InteractWithTextAsset { get; set; }
        public InteractWithMonoTextAssetContext InteractWithMonoTextAsset { get; set; }
        public InteractWithFileTextContext InteractWithFileText { get; set; }
        public TextAssetTabControlContext()
        {
            InteractWithTextAsset = new InteractWithTextAssetContext();
            InteractWithMonoTextAsset = new InteractWithMonoTextAssetContext();
            InteractWithFileText = new InteractWithFileTextContext();
        }
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

            LoadUnityL10nTool_BackgroundWorker.ReportProgress(0, "Initialize UnityL10nTool...");
            unityL10nToolCppManaged = new UnityL10nToolCppManaged(unityL10NToolProjectInfo.JSONPath.Replace("setting.json", ""));

            LoadUnityL10nTool_BackgroundWorker.ReportProgress(0, "Loading Assets...");
            unityL10nToolCppManaged.LoadGlobalgamemanagersFile(); 

            LoadUnityL10nTool_BackgroundWorker.ReportProgress(0, "Loading Class Information...");
            unityL10nToolCppManaged.ProcessResourceAndMonoManger(); 

            LoadUnityL10nTool_BackgroundWorker.ReportProgress(0, "Loading MonoClassDatabase...");
            unityL10nToolCppManaged.LoadMonoClassDatabase();

            LoadUnityL10nTool_BackgroundWorker.ReportProgress(0, "Loading UnityL10nToolAPI...");
            unityL10nToolCppManaged.LoadUnityL10nToolAPI();

            LoadUnityL10nTool_BackgroundWorker.ReportProgress(0, "Loading Font Plugins...");
            List<string> loadedFontPlugins = unityL10nToolCppManaged.LoadFontPlugins();

            LoadUnityL10nTool_BackgroundWorker.ReportProgress(0, "Loading Font Plugins Support Asset List...");
            pluginsSupportAssetMap = unityL10nToolCppManaged.GetPluginsSupportAssetMap();

            LoadUnityL10nTool_BackgroundWorker.ReportProgress(0, "Loading Text Plugins Support Asset List...");
            List<TextAssetMapCLI> textAssetMapCLIs = unityL10nToolCppManaged.GetTextAssetMaps();
            TextAssetTabControlContext textAssetTabControlContextLocal = new TextAssetTabControlContext();
            textAssetMapCLIs.ForEach(x => textAssetTabControlContextLocal.InteractWithTextAsset.News.Add(new KeyValuePair<string,TextAssetMapCLI>(unityL10nToolCppManaged.GetOriginalText(x), x)));
            textAssetTabControlContext = textAssetTabControlContextLocal;

            LoadUnityL10nTool_BackgroundWorker.ReportProgress(0, "Loading Text Plugins Support Asset List...");
            bool resultload = unityL10nToolCppManaged.LoadTextPlugins();
            interactWithAssetNames = unityL10nToolCppManaged.GetInteractWithAssetPluginNames();
            //interactWithAssetNames.Insert(0, null);
            interactWithAssetNames.Insert(0, "");
            interactWithFileTextNames = unityL10nToolCppManaged.GetInteractWithFileTextPluginNames();
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
            TextAssetTabControl.DataContext = textAssetTabControlContext;
            InteractWithAssetCombobox.ItemsSource = interactWithAssetNames;
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
            try
            {
                FontAssetMapCLI found = fontAssetMapsCLI.Saveds.Single(x =>
                    x.assetName == fontAssetMapCLI.assetName &&
                    x.assetsName == fontAssetMapCLI.assetsName &&
                    x.containerPath == fontAssetMapCLI.containerPath);
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
                    x.assetName == fontAssetMapCLI.assetName &&
                    x.assetsName == fontAssetMapCLI.assetsName &&
                    x.containerPath == fontAssetMapCLI.containerPath);
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
                    x.assetName == fontAssetMapCLI.assetName &&
                    x.assetsName == fontAssetMapCLI.assetsName &&
                    x.containerPath == fontAssetMapCLI.containerPath);
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
        }

        private void Button_Click_3(object sender, RoutedEventArgs e)
        {
            unityL10nToolCppManaged.SetPluginsSupportAssetMap(pluginsSupportAssetMap);
            unityL10nToolCppManaged.GetProjectConfigJsonFromFontPlugin();
            unityL10nToolCppManaged.SaveProjectConfigJson();
        }

        private void Button_Click_4(object sender, RoutedEventArgs e)
        {
            unityL10nToolCppManaged.BuildProject(unityL10NToolProjectInfo.JSONPath.Replace("setting.json", "Build\\"));
        }

        private void CustomProperties_DataContextChanged(object sender, DependencyPropertyChangedEventArgs e)
        {

        }

        private void InteractWithTextAssetDataGridNews_SelectedCellsChanged(object sender, SelectedCellsChangedEventArgs e)
        {
            if(sender is DataGrid dataGrid)
            {
                if(dataGrid.SelectedItem is TextAssetMapCLI textAssetMapCLI)
                {
                    unityL10nToolCppManaged.GetOriginalLanguagePairDics(textAssetMapCLI);
                }
            }
        }

        // deprecated
        private void InteractWithAssetCombobox_DataContextChanged(object sender, DependencyPropertyChangedEventArgs e)
        {
            
        }

        private void InteractWithAssetCombobox_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            if (sender is ComboBox comboBox)
            {
                if (comboBox.DataContext is TextAssetMapCLI textAssetMapCLI)
                {
                    TextAssetMapCLI updatedTextAssetMapCLI = unityL10nToolCppManaged.GetOriginalLanguagePairDics(textAssetMapCLI);
                    var found = textAssetTabControlContext.InteractWithTextAsset.News.First(x => x.Value == textAssetMapCLI);
                    int i = textAssetTabControlContext.InteractWithTextAsset.News.IndexOf(found);
                    comboBox.SelectionChanged -= InteractWithAssetCombobox_SelectionChanged;
                    textAssetTabControlContext.InteractWithTextAsset.News[i]
                        = new KeyValuePair<string, TextAssetMapCLI>(
                            textAssetTabControlContext.InteractWithTextAsset.News[i].Key
                            ,updatedTextAssetMapCLI);
                    textAssetTabControlContext.InteractWithTextAsset.SelectedItem = textAssetTabControlContext.InteractWithTextAsset.News[i];
                    comboBox.SelectionChanged += InteractWithAssetCombobox_SelectionChanged;
                }
            }
        }

        private void InteractWithTextAssetPropertyies_DataContextChanged(object sender, DependencyPropertyChangedEventArgs e)
        {

        }

        private void InteractWithTextAssetPropertyies_OptionChanged(object sender, DependencyPropertyChangedEventArgs e)
        {
            if(sender is InteractWithTextAssetPropertyies interactWithTextAssetPropertyies)
            {
                if(interactWithTextAssetPropertyies.DataContext is TextAssetMapCLI textAssetMapCLI)
                {
                    TextAssetMapCLI updatedTextAssetMapCLI = unityL10nToolCppManaged.GetOriginalLanguagePairDics(textAssetMapCLI);
                    var found = textAssetTabControlContext.InteractWithTextAsset.News.First(x => x.Value == textAssetMapCLI);
                    int i = textAssetTabControlContext.InteractWithTextAsset.News.IndexOf(found);
                    textAssetTabControlContext.InteractWithTextAsset.News[i] 
                        = new KeyValuePair<string, TextAssetMapCLI>(
                            textAssetTabControlContext.InteractWithTextAsset.News[i].Key
                            , updatedTextAssetMapCLI);
                    textAssetTabControlContext.InteractWithTextAsset.SelectedItem = textAssetTabControlContext.InteractWithTextAsset.News[i];
                }
            }
        }
    }
}
