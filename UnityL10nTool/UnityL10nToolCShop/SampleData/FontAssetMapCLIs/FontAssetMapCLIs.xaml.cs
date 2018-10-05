//      *********    이 파일을 수정하지 마십시오.     *********
//      이 파일은 디자인 도구에서 다시 생성했습니다. 이 파일을
//      변경하면 오류가 발생할 수 있습니다.
namespace Expression.Blend.SampleData.FontAssetMapCLIs
{
    using System; 
    using System.ComponentModel;

// 프로덕션 응용 프로그램에서 예제 데이터 사용 공간을 대폭 줄이려면
// 런타임에 DISABLE_SAMPLE_DATA 조건부 컴파일 상수를 설정하고 예제 데이터를 사용하지 않도록 설정하면 됩니다.
#if DISABLE_SAMPLE_DATA
    internal class FontAssetMapCLIs { }
#else

    public class FontAssetMapCLIs : INotifyPropertyChanged
    {
        public event PropertyChangedEventHandler PropertyChanged;

        protected virtual void OnPropertyChanged(string propertyName)
        {
            if (this.PropertyChanged != null)
            {
                this.PropertyChanged(this, new PropertyChangedEventArgs(propertyName));
            }
        }

        public FontAssetMapCLIs()
        {
            try
            {
                Uri resourceUri = new Uri("/UnityL10nToolCShop;component/SampleData/FontAssetMapCLIs/FontAssetMapCLIs.xaml", UriKind.RelativeOrAbsolute);
                System.Windows.Application.LoadComponent(this, resourceUri);
            }
            catch
            {
            }
        }

        private ItemCollection _Collection = new ItemCollection();

        public ItemCollection Collection
        {
            get
            {
                return this._Collection;
            }
        }
    }

    public class ItemCollection : System.Collections.ObjectModel.ObservableCollection<Item>
    { 
    }

    public class Item : INotifyPropertyChanged
    {
        public event PropertyChangedEventHandler PropertyChanged;

        protected virtual void OnPropertyChanged(string propertyName)
        {
            if (this.PropertyChanged != null)
            {
                this.PropertyChanged(this, new PropertyChangedEventArgs(propertyName));
            }
        }

        private string _Key = string.Empty;

        public string Key
        {
            get
            {
                return this._Key;
            }

            set
            {
                if (this._Key != value)
                {
                    this._Key = value;
                    this.OnPropertyChanged("Key");
                }
            }
        }

        private Value _Value = new Value();

        public Value Value
        {
            get
            {
                return this._Value;
            }
        }
    }

    public class optionsItem1 : INotifyPropertyChanged
    {
        public event PropertyChangedEventHandler PropertyChanged;

        protected virtual void OnPropertyChanged(string propertyName)
        {
            if (this.PropertyChanged != null)
            {
                this.PropertyChanged(this, new PropertyChangedEventArgs(propertyName));
            }
        }

        private string _test1 = string.Empty;

        public string test1
        {
            get
            {
                return this._test1;
            }

            set
            {
                if (this._test1 != value)
                {
                    this._test1 = value;
                    this.OnPropertyChanged("test1");
                }
            }
        }

        private string _Property2 = string.Empty;

        public string Property2
        {
            get
            {
                return this._Property2;
            }

            set
            {
                if (this._Property2 != value)
                {
                    this._Property2 = value;
                    this.OnPropertyChanged("Property2");
                }
            }
        }
    }

    public class Value : System.Collections.ObjectModel.ObservableCollection<ValueItem>
    { 
    }

    public class ValueItem : INotifyPropertyChanged
    {
        public event PropertyChangedEventHandler PropertyChanged;

        protected virtual void OnPropertyChanged(string propertyName)
        {
            if (this.PropertyChanged != null)
            {
                this.PropertyChanged(this, new PropertyChangedEventArgs(propertyName));
            }
        }

        private string _assetsName = string.Empty;

        public string assetsName
        {
            get
            {
                return this._assetsName;
            }

            set
            {
                if (this._assetsName != value)
                {
                    this._assetsName = value;
                    this.OnPropertyChanged("assetsName");
                }
            }
        }

        private string _assetName = string.Empty;

        public string assetName
        {
            get
            {
                return this._assetName;
            }

            set
            {
                if (this._assetName != value)
                {
                    this._assetName = value;
                    this.OnPropertyChanged("assetName");
                }
            }
        }

        private string _containerPath = string.Empty;

        public string containerPath
        {
            get
            {
                return this._containerPath;
            }

            set
            {
                if (this._containerPath != value)
                {
                    this._containerPath = value;
                    this.OnPropertyChanged("containerPath");
                }
            }
        }

        private options1 _options = new options1();

        public options1 options
        {
            get
            {
                return this._options;
            }
        }

        private string _selectedOption = string.Empty;

        public string selectedOption
        {
            get
            {
                return this._selectedOption;
            }

            set
            {
                if (this._selectedOption != value)
                {
                    this._selectedOption = value;
                    this.OnPropertyChanged("selectedOption");
                }
            }
        }

        private bool _useContainerPath = false;

        public bool useContainerPath
        {
            get
            {
                return this._useContainerPath;
            }

            set
            {
                if (this._useContainerPath != value)
                {
                    this._useContainerPath = value;
                    this.OnPropertyChanged("useContainerPath");
                }
            }
        }
    }

    public class options : System.Collections.ObjectModel.ObservableCollection<optionsItem>
    { 
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

        private options _options = new options();

        public options options
        {
            get
            {
                return this._options;
            }
        }
    }

    public class options1 : System.Collections.ObjectModel.ObservableCollection<optionsItem1>
    { 
    }
#endif
}
