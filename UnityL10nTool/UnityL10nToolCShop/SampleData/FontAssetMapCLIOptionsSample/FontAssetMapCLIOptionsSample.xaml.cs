//      *********    이 파일을 수정하지 마십시오.     *********
//      이 파일은 디자인 도구에서 다시 생성했습니다. 이 파일을
//      변경하면 오류가 발생할 수 있습니다.
namespace Expression.Blend.SampleData.FontAssetMapCLIOptionsSample
{
    using System; 
    using System.ComponentModel;

// 프로덕션 응용 프로그램에서 예제 데이터 사용 공간을 대폭 줄이려면
// 런타임에 DISABLE_SAMPLE_DATA 조건부 컴파일 상수를 설정하고 예제 데이터를 사용하지 않도록 설정하면 됩니다.
#if DISABLE_SAMPLE_DATA
    internal class FontAssetMapCLIOptionsSample { }
#else

    public class FontAssetMapCLIOptionsSample : INotifyPropertyChanged
    {
        public event PropertyChangedEventHandler PropertyChanged;

        protected virtual void OnPropertyChanged(string propertyName)
        {
            if (this.PropertyChanged != null)
            {
                this.PropertyChanged(this, new PropertyChangedEventArgs(propertyName));
            }
        }

        public FontAssetMapCLIOptionsSample()
        {
            try
            {
                Uri resourceUri = new Uri("/UnityL10nToolCShop;component/SampleData/FontAssetMapCLIOptionsSample/FontAssetMapCLIOptionsSample.xaml", UriKind.RelativeOrAbsolute);
                System.Windows.Application.LoadComponent(this, resourceUri);
            }
            catch
            {
            }
        }

        private options _options = new options();

        public options options
        {
            get
            {
                return this._options;
            }
        }
    }

    public class optionsItem : INotifyPropertyChanged
    {
        public event PropertyChangedEventHandler PropertyChanged;

        protected virtual void OnPropertyChanged(string propertyName)
        {
            if (this.PropertyChanged != null)
            {
                this.PropertyChanged(this, new PropertyChangedEventArgs(propertyName));
            }
        }

        private string _options = string.Empty;

        public string options
        {
            get
            {
                return this._options;
            }

            set
            {
                if (this._options != value)
                {
                    this._options = value;
                    this.OnPropertyChanged("options");
                }
            }
        }
    }

    public class options : System.Collections.ObjectModel.ObservableCollection<optionsItem>
    { 
    }
#endif
}
