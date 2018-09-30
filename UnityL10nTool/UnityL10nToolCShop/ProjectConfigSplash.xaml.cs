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

namespace UnityL10nToolCShop
{
    /// <summary>
    /// ProjectConfigSplash.xaml에 대한 상호 작용 논리
    /// </summary>
    public partial class ProjectConfigSplash : UserControl
    {
        public ProjectConfigSplash()
        {
            InitializeComponent();
        }

        public string ProgressText {
            set
            {
                ProgressText1.Content = value;
            }
            get
            {
                return ProgressText1.Content.ToString();
            }
        }
    }
}
