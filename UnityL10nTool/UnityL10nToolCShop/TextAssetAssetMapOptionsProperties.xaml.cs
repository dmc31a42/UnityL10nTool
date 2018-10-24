using System;
using System.Collections.Generic;
using System.Linq;
using System.Text.RegularExpressions;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Input;
using UnityL10nToolCppCLI;

namespace UnityL10nToolCShop
{
    /// <summary>
    /// CustomProperties.xaml에 대한 상호 작용 논리
    /// </summary>
    public partial class TextAssetAssetMapOptionsProperties : UserControl
    {
        // https://stackoverflow.com/questions/25895011/how-to-add-custom-properties-to-wpf-user-control
        public bool IsReadOnly { get; set; }
        public static readonly DependencyProperty IsReadOnlyProperty
            = DependencyProperty.Register(
                "IsReadOnly",
                typeof(bool),
                typeof(InteractWithTextAssetProperties),
                new PropertyMetadata(false));
        List<AssetMapOptionCLI> AssetMapOptionCLIsGlobal;
        public event DependencyPropertyChangedEventHandler OptionChanged;
        public TextAssetAssetMapOptionsProperties()
        {
            InitializeComponent();
            DataContextChanged += CustomProperties_DataContextChanged;
        }

        public void Refresh()
        {
            PropertiesStackPanel.Children.Clear();
            for (int i = 0; i < AssetMapOptionCLIsGlobal.Count; i++)
            {
                AssetMapOptionCLI child = AssetMapOptionCLIsGlobal[i];
                SetPropertyControlRecursive(ref child);
            }
            return;
        }

        private void CustomProperties_DataContextChanged(object sender, DependencyPropertyChangedEventArgs e)
        {
            if (DataContext is List<AssetMapOptionCLI> assetMapOptionCLIs)
            {
                AssetMapOptionCLIsGlobal = assetMapOptionCLIs;
            }
            else
            {

            }
        }

        public void SetPropertyControlStatic(string key, object value)
        {
            Grid grid = new Grid
            {
                Margin = new Thickness(5)
            };
            grid.GotFocus += Grid_GotFocus;
            ColumnDefinition columnDefinition1 = new ColumnDefinition
            {
                Width = new GridLength(4, GridUnitType.Star)
            };
            ColumnDefinition columnDefinition2 = new ColumnDefinition
            {
                Width = new GridLength(6, GridUnitType.Star)
            };
            grid.ColumnDefinitions.Add(columnDefinition1);
            grid.ColumnDefinitions.Add(columnDefinition2);
            TextBlock textBlock1 = new TextBlock();
            textBlock1.ToolTip = key + "\r\n" + value;
            Grid.SetColumn(textBlock1, 0);
            textBlock1.Text = key;
            textBlock1.VerticalAlignment = VerticalAlignment.Center;
            grid.Children.Add(textBlock1);
            if(value is string)
            {
                string valueAsString = value as string;
                TextBlock textBlock2 = new TextBlock();
                Grid.SetColumn(textBlock2, 1);
                grid.Children.Add(textBlock2);
                textBlock2.Text = valueAsString;
                textBlock2.TextWrapping = TextWrapping.Wrap;
            } else if(value is bool)
            {
                bool valueAsBool = (bool)value;
                CheckBox checkBox = new CheckBox();
                Grid.SetColumn(checkBox, 1);
                Binding binding = new Binding("useContainerPath");
                checkBox.SetBinding(CheckBox.IsCheckedProperty, binding);
                grid.Children.Add(checkBox);
            }
            PropertiesStackPanel.Children.Add(grid);
            
        }

        public void SetPropertyControlRecursive(ref AssetMapOptionCLI fontAssetMapOptionCLI)
        {
            if (fontAssetMapOptionCLI.type == AssetMapOptionCLI.Type.OPTION_TYPE_NONE)
            {
                return;
            }
            Grid grid = new Grid
            {
                DataContext = fontAssetMapOptionCLI,
                Margin = new Thickness(5)
            };
            grid.GotFocus += Grid_GotFocus;
            ColumnDefinition columnDefinition1 = new ColumnDefinition
            {
                Width = new GridLength(4, GridUnitType.Star)
            };
            ColumnDefinition columnDefinition2 = new ColumnDefinition
            {
                Width = new GridLength(6, GridUnitType.Star)
            };
            grid.ColumnDefinitions.Add(columnDefinition1);
            grid.ColumnDefinitions.Add(columnDefinition2);
            TextBlock textBlock1 = new TextBlock();
            textBlock1.ToolTip = fontAssetMapOptionCLI.OptionName + "\r\n" + fontAssetMapOptionCLI.OptionDescription;
            Grid.SetColumn(textBlock1, 0);
            textBlock1.Text = fontAssetMapOptionCLI.OptionName;
            textBlock1.VerticalAlignment = VerticalAlignment.Center;
            grid.Children.Add(textBlock1);
            PropertiesStackPanel.Children.Add(grid);

            if ((fontAssetMapOptionCLI.nestedOptions == null || fontAssetMapOptionCLI.nestedOptions.Count == 0) ||
                (fontAssetMapOptionCLI.nestedOptions != null && fontAssetMapOptionCLI.nestedOptions.Count == 1 
                && fontAssetMapOptionCLI.nestedOptions[0].typeAsChild == AssetMapOptionCLI.Type.OPTION_TYPE_NONE))
            {
                Binding binding = new Binding("Value")
                {
                    Mode = BindingMode.TwoWay
                };

                switch (fontAssetMapOptionCLI.type)
                {
                    case AssetMapOptionCLI.Type.OPTION_TYPE_STRING:
                        TextBox textBox1 = new TextBox();
                        textBox1.SetBinding(TextBox.TextProperty, binding);
                        textBox1.IsEnabled = !IsReadOnly;
                        textBox1.TextChanged += TextBox_TextChangedAsync;
                        Grid.SetColumn(textBox1, 1);
                        grid.Children.Add(textBox1);
                        break;
                    case AssetMapOptionCLI.Type.OPTION_TYPE_INT:
                        TextBox textBox2 = new TextBox();
                        textBox2.PreviewTextInput += TextBoxInteger_PreviewTextInput;
                        textBox2.SetBinding(TextBox.TextProperty, binding);
                        textBox2.IsEnabled = !IsReadOnly;
                        textBox2.TextChanged += TextBox_TextChangedAsync;
                        Grid.SetColumn(textBox2, 1);
                        grid.Children.Add(textBox2);
                        break;
                    case AssetMapOptionCLI.Type.OPTION_TYPE_DOUBLE:
                        TextBox textBox3 = new TextBox();
                        textBox3.PreviewTextInput += TextBoxDouble_previewTextInput;
                        textBox3.SetBinding(TextBox.TextProperty, binding);
                        textBox3.IsEnabled = !IsReadOnly;
                        textBox3.TextChanged += TextBox_TextChangedAsync;
                        Grid.SetColumn(textBox3, 1);
                        grid.Children.Add(textBox3);

                        break;
                    case AssetMapOptionCLI.Type.OPTION_TYPE_BOOL:
                        CheckBox checkBox1 = new CheckBox();
                        checkBox1.SetBinding(CheckBox.IsCheckedProperty, binding);
                        checkBox1.IsEnabled = !IsReadOnly;
                        checkBox1.Checked += CheckBox_Checked;
                        checkBox1.Unchecked += CheckBox_Checked;
                        Grid.SetColumn(checkBox1, 1);
                        grid.Children.Add(checkBox1);
                        break;
                }
                if((fontAssetMapOptionCLI.nestedOptions != null && fontAssetMapOptionCLI.nestedOptions.Count == 1
                && fontAssetMapOptionCLI.nestedOptions[0].typeAsChild == AssetMapOptionCLI.Type.OPTION_TYPE_NONE))
                {
                    AssetMapOptionCLI child = fontAssetMapOptionCLI.nestedOptions[0];
                    SetPropertyControlRecursive(ref child);
                }
            }
            else
            {
                Binding binding = new Binding("Value")
                {
                    Mode = BindingMode.TwoWay
                };
                switch (fontAssetMapOptionCLI.type)
                {
                    case AssetMapOptionCLI.Type.OPTION_TYPE_STRING:
                        ComboBox comboBox1 = new ComboBox();
                        Grid.SetColumn(comboBox1, 1);
                        List<string> vs1 = new List<string>();
                        foreach (AssetMapOptionCLI fontAssetMapOptionCLIChild in fontAssetMapOptionCLI.nestedOptions)
                        {
                            if (fontAssetMapOptionCLIChild.ValueAsChild != null)
                            {
                                vs1.Add((string)fontAssetMapOptionCLIChild.ValueAsChild);
                            }
                        }
                        comboBox1.ItemsSource = vs1;
                        comboBox1.SetBinding(ComboBox.SelectedItemProperty, binding);
                        comboBox1.IsEnabled = !IsReadOnly;
                        grid.Children.Add(comboBox1);
                        if (fontAssetMapOptionCLI.Value != null)
                        {
                            string valueAsChild = (string)fontAssetMapOptionCLI.Value;
                            int findIndex = fontAssetMapOptionCLI.nestedOptions.FindIndex(x => (string)x.ValueAsChild == valueAsChild);
                            if (findIndex > -1)
                            {
                                AssetMapOptionCLI found = fontAssetMapOptionCLI.nestedOptions[findIndex];
                                comboBox1.SelectedItem = (string)(found).ValueAsChild;
                                SetPropertyControlRecursive(ref found);
                            }
                            else
                            {
                                comboBox1.IsEditable = true;
                                comboBox1.IsReadOnly = true;
                                comboBox1.Text = "(Select " + fontAssetMapOptionCLI.OptionName + ")";
                            }
                        }
                        else
                        {
                            comboBox1.IsEditable = true;
                            comboBox1.IsReadOnly = true;
                            comboBox1.Text = "(Select " + fontAssetMapOptionCLI.OptionName + ")";
                        }
                        comboBox1.SelectionChanged += ComboBox_SelectionChanged;

                        break;
                    case AssetMapOptionCLI.Type.OPTION_TYPE_INT:
                        ComboBox comboBox2 = new ComboBox();
                        comboBox2.SetBinding(ComboBox.SelectedItemProperty, binding);
                        comboBox2.IsEnabled = !IsReadOnly;
                        Grid.SetColumn(comboBox2, 1);
                        List<int> vs2 = new List<int>();
                        foreach (AssetMapOptionCLI fontAssetMapOptionCLIChild in fontAssetMapOptionCLI.nestedOptions)
                        {
                            if (fontAssetMapOptionCLIChild.ValueAsChild != null)
                            {
                                vs2.Add((int)fontAssetMapOptionCLIChild.ValueAsChild);
                            }
                        }
                        comboBox2.ItemsSource = vs2;
                        comboBox2.SetBinding(ComboBox.SelectedItemProperty, binding);
                        grid.Children.Add(comboBox2);
                        if (fontAssetMapOptionCLI.Value != null)
                        {
                            int valueAsChild = (int)fontAssetMapOptionCLI.Value;
                            int findIndex = fontAssetMapOptionCLI.nestedOptions.FindIndex(x => (int)x.ValueAsChild == valueAsChild);
                            if (findIndex > -1)
                            {
                                AssetMapOptionCLI found = fontAssetMapOptionCLI.nestedOptions[findIndex];
                                comboBox2.SelectedItem = (int)(found).ValueAsChild;
                                SetPropertyControlRecursive(ref found);
                            }
                            else
                            {
                                comboBox2.IsEditable = true;
                                comboBox2.IsReadOnly = true;
                                comboBox2.Text = "(Select " + fontAssetMapOptionCLI.OptionName + ")";
                            }
                        }
                        else
                        {
                            comboBox2.IsEditable = true;
                            comboBox2.IsReadOnly = true;
                            comboBox2.Text = "(Select " + fontAssetMapOptionCLI.OptionName + ")";
                        }
                        comboBox2.SelectionChanged += ComboBox_SelectionChanged;
                        break;
                    case AssetMapOptionCLI.Type.OPTION_TYPE_DOUBLE:
                        ComboBox comboBox3 = new ComboBox();
                        comboBox3.SetBinding(ComboBox.SelectedItemProperty, binding);
                        comboBox3.IsEnabled = !IsReadOnly;
                        Grid.SetColumn(comboBox3, 1);
                        List<double> vs3 = new List<double>();
                        foreach (AssetMapOptionCLI fontAssetMapOptionCLIChild in fontAssetMapOptionCLI.nestedOptions)
                        {
                            if (fontAssetMapOptionCLIChild.ValueAsChild != null)
                            {
                                vs3.Add((double)fontAssetMapOptionCLIChild.ValueAsChild);
                            }
                        }
                        comboBox3.ItemsSource = vs3;
                        comboBox3.SetBinding(ComboBox.SelectedItemProperty, binding);
                        grid.Children.Add(comboBox3);
                        if (fontAssetMapOptionCLI.Value != null)
                        {
                            double valueAsChild = (double)fontAssetMapOptionCLI.Value;
                            int findIndex = fontAssetMapOptionCLI.nestedOptions.FindIndex(x => (double)x.ValueAsChild == valueAsChild);
                            if (findIndex > -1)
                            {
                                AssetMapOptionCLI found = fontAssetMapOptionCLI.nestedOptions[findIndex];
                                comboBox3.SelectedItem = (int)(found).ValueAsChild;
                                SetPropertyControlRecursive(ref found);
                            }
                            else
                            {
                                comboBox3.IsEditable = true;
                                comboBox3.IsReadOnly = true;
                                comboBox3.Text = "(Select " + fontAssetMapOptionCLI.OptionName + ")";
                            }
                        }
                        else
                        {
                            comboBox3.IsEditable = true;
                            comboBox3.IsReadOnly = true;
                            comboBox3.Text = "(Select " + fontAssetMapOptionCLI.OptionName + ")";
                        }
                        comboBox3.SelectionChanged += ComboBox_SelectionChanged;
                        break;
                    case AssetMapOptionCLI.Type.OPTION_TYPE_BOOL:
                        CheckBox checkBox1 = new CheckBox();
                        checkBox1.SetBinding(CheckBox.IsCheckedProperty, binding);
                        checkBox1.IsEnabled = !IsReadOnly;
                        Grid.SetColumn(checkBox1, 1);
                        grid.Children.Add(checkBox1);
                        if (fontAssetMapOptionCLI.Value != null)
                        {
                            bool valueAsChild = (bool)fontAssetMapOptionCLI.Value;
                            int findIndex = fontAssetMapOptionCLI.nestedOptions.FindIndex(x => (bool)x.ValueAsChild == valueAsChild);
                            if (findIndex > -1)
                            {
                                AssetMapOptionCLI found = fontAssetMapOptionCLI.nestedOptions[findIndex];
                                checkBox1.IsChecked = (bool)(found).ValueAsChild;
                                SetPropertyControlRecursive(ref found);
                            }
                            else
                            {
                                checkBox1.IsChecked = false;
                            }
                        }
                        else
                        {
                            checkBox1.IsChecked = false;
                        }
                        checkBox1.Checked += CheckBox_Checked;
                        checkBox1.Unchecked += CheckBox_Checked;
                        break;
                }
            }
        }

        private async void TextBox_TextChangedAsync(object sender, TextChangedEventArgs e)
        {
            if(sender is TextBox textBox)
            {
                if(e.Changes.Count == 1 && e.Changes.First().AddedLength != textBox.Text.Count())
                {
                    string startStr = textBox.Text;

                    await Task.Delay(500);
                    if (startStr == textBox.Text)
                    {
                        if(textBox.DataContext is AssetMapOptionCLI assetMapOptionCLI)
                        {
                            assetMapOptionCLI.Value = textBox.Text;
                            OptionChanged(this, new DependencyPropertyChangedEventArgs());
                        }
                    }
                }
            }
        }

        private void Grid_GotFocus(object sender, RoutedEventArgs e)
        {
            Grid grid = (Grid)sender;
            HelpStackPanel.DataContext = grid.DataContext;
        }

        private void CheckBox_Checked(object sender, RoutedEventArgs e)
        {
            OptionChanged(this, new DependencyPropertyChangedEventArgs());
        }

        private void ComboBox_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            OptionChanged(this, new DependencyPropertyChangedEventArgs());
        }

        // https://twinparadox.tistory.com/404
        private void TextBoxInteger_PreviewTextInput(object sender, TextCompositionEventArgs e)
        {
            Regex regex = new Regex("[^0-9]+");
            e.Handled = regex.IsMatch(e.Text);
        }

        private void TextBoxDouble_previewTextInput(object sender, TextCompositionEventArgs e)
        {
            double d1;
            if (Double.TryParse(e.Text, out d1))
            {
                e.Handled = true;
            }
            else
            {
                e.Handled = false;
            }
        }

        private void Grid_DataContextChanged(object sender, DependencyPropertyChangedEventArgs e)
        {
            if(sender is Grid grid)
            {
                PropertiesStackPanel.Children.Clear();
                if (grid.DataContext is List<AssetMapOptionCLI> assetMapOptionCLIs)
                {
                    for (int i = 0; i < assetMapOptionCLIs.Count; i++)
                    {
                        AssetMapOptionCLI child = assetMapOptionCLIs[i];
                        SetPropertyControlRecursive(ref child);
                    }
                }
            } 
        }
    }
}
