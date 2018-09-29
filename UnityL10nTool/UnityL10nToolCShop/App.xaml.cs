using System;
using System.Collections.Generic;
using System.Configuration;
using System.Data;
using System.Linq;
using System.Windows;
using UnityL10nToolCppCLI;

namespace UnityL10nToolCShop
{
    /// <summary>
    /// App.xaml에 대한 상호 작용 논리
    /// </summary>
    public partial class App : Application
    {
    }

    public class UnityL10nToolProjectInfo
    {
        public string GameName { set; get; }
        public string MakerName { set; get; }
        public string GamePath { set; get; }
        public string JSONPath { set; get; }
        public string DataFolderName { set; get; }
        public UnityL10nToolProjectInfo(string GameName, string MakerName, string GamePath, string JSONPath, string DataFolderName)
        {
            this.GameName = GameName;
            this.MakerName = MakerName;
            this.GamePath = GamePath;
            this.JSONPath = JSONPath;
            this.DataFolderName = DataFolderName;
        }
    }
}
