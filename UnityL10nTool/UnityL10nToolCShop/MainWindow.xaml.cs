using System;
using System.Collections.Generic;
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

namespace UnityL10nToolCShop
{
    /// <summary>
    /// MainWindow.xaml에 대한 상호 작용 논리
    /// </summary>
    public partial class MainWindow : Window
    {
        List<string> projectList = new List<string>();

        public MainWindow()
        {
            InitializeComponent();

            // http://infodbbase.tistory.com/105
            if (System.IO.Directory.Exists("Projects"))
            {
                System.IO.DirectoryInfo di = new System.IO.DirectoryInfo("Projects");

                foreach(var directory in di.GetDirectories())
                {
                    FileInfo[] settingFileInfos = directory.GetFiles("setting.json");
                    if(settingFileInfos.Length == 1)
                    {
                        projectList.Add(directory.Name);
                    }
                }
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
            UnityL10nToolCppManaged unityL10NToolCppManaged = ((App)Application.Current).unityL10NToolCppManaged = new UnityL10nToolCppManaged(gameFolderPath.Text);
            Dictionary<string, List<FontAssetMapCLI>> directory = unityL10NToolCppManaged.GetPluginsSupportAssetMap();
        }
    }
}
