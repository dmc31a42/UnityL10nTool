﻿using System;
using System.Collections.Generic;
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

        public ProjectConfig(UnityL10nToolProjectInfo unityL10NToolProjectInfo)
        {
            InitializeComponent();
            this.unityL10NToolProjectInfo = unityL10NToolProjectInfo;
        }

        private void Window_Loaded(object sender, RoutedEventArgs e)
        {
            projectConfigSplash = new ProjectConfigSplash();
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
            
            LoadUnityL10nTool_BackgroundWorker.ReportProgress(0, "Initialize UnityL10nTool...");
            unityL10nToolCppManaged = new UnityL10nToolCppManaged(gamePath);

            LoadUnityL10nTool_BackgroundWorker.ReportProgress(0, "Loading Assets...");
            unityL10nToolCppManaged.LoadGlobalgamemanagersFile();

            LoadUnityL10nTool_BackgroundWorker.ReportProgress(0, "Loading UnityL10nToolAPI...");
            unityL10nToolCppManaged.LoadUnityL10nToolAPI();

            LoadUnityL10nTool_BackgroundWorker.ReportProgress(0, "Loading Font Plugins...");
            List<string> loadedFontPlugins = unityL10nToolCppManaged.LoadFontPlugins();

            LoadUnityL10nTool_BackgroundWorker.ReportProgress(0, "Loading Plugins Support Asset List...");
            Dictionary<string, List<FontAssetMapCLI>> pluginsSupportAssetMap = unityL10nToolCppManaged.GetPluginsSupportAssetMap();
        }

        private void LoadUnityL10nTool_ProgressChanged(object sender, ProgressChangedEventArgs e)
        {
            projectConfigSplash.ProgressText = (string)e.UserState;
        }

        private void LoadUnityL10nTool_RunWorkerCompleted(object sender, RunWorkerCompletedEventArgs e)
        {
            MainGrid.Children.Remove(projectConfigSplash);
        }
    }
}
