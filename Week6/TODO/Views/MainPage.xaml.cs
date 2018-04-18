using SQLitePCL;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using System.Text;
using TODO.Models;
using TODO.Services;
using TODO.ViewModels;
using Windows.ApplicationModel;
using Windows.ApplicationModel.DataTransfer;
using Windows.Data.Xml.Dom;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.Storage;
using Windows.Storage.Pickers;
using Windows.Storage.Streams;
using Windows.UI.Notifications;
using Windows.UI.Popups;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Media.Imaging;
using Windows.UI.Xaml.Navigation;

// https://go.microsoft.com/fwlink/?LinkId=402352&clcid=0x804 上介绍了“空白页”项模板

namespace TODO
{
    /// <summary>
    /// 可用于自身或导航至 Frame 内部的空白页。
    /// </summary>
    public sealed partial class MainPage : Page
    {
        public enum ModeChangeArgs { CreateToChange, ChangeToCreate };
        public static MainPage Current;
        public ItemsManager VM;

        private TodoItem _selecteditem;
        public TodoItem SelectedItem
        {
            get { return _selecteditem; }
            set
            {
                _selecteditem = value;
                SelectedIndex = VM.Collection.IndexOf(_selecteditem);
            }
        }
        
        
        public int SelectedIndex;
        public string ImageUrl;
        public int DetailMode = 1; // 1为new，0为Detail
        DataTransferManager dataTransferManager = DataTransferManager.GetForCurrentView();

        public MainPage()
        {
            this.InitializeComponent();
            Current = this;
            VM = ItemsManager.Instance;
            this.SizeChanged += (s, e) =>
            {
                MainListView.Header = Window.Current.Bounds.Width.ToString();
            };
        }

        private void ListView_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            SelectedItem = MainListView.SelectedItem as TodoItem;
            SelectedIndex = MainListView.SelectedIndex;
            if (Window.Current.Bounds.Width >= 800)
            {
                SetDetailInfomation();
            }
            else
            {
                (Window.Current.Content as Frame).Navigate(typeof(Views.DetailsPage));
            }            
        }

        /// <summary>
        /// 对于detail部分进行重置
        /// </summary>
        /// <param name="mode">0代表重置，1代表进入new模式</param>
        public void SetDetailInfomation(int mode = 0)
        {
            if (SelectedItem == null || mode == 1)
            {
                DetailsTitleTextBox.Text = DetailsDescriptionTextBox.Text = "";
                DueDatePicker.Date = DateTimeOffset.Now;
                ImageControl.Source = new BitmapImage(new Uri("ms-appx:///Assets/icons8-task-50.png", UriKind.Absolute));
                DetailModeChange(ModeChangeArgs.ChangeToCreate);
            }
            else
            {
                DetailsTitleTextBox.Text = SelectedItem.Title;
                DetailsDescriptionTextBox.Text = SelectedItem.Description;
                DueDatePicker.Date = SelectedItem.DueDate;
                ImageControl.Source = SelectedItem.Image;
                DetailModeChange(ModeChangeArgs.CreateToChange);
            }
            
        }

        private void DetailModeChange(ModeChangeArgs e)
        {
            if (e == ModeChangeArgs.CreateToChange)
            {
                if (DetailMode == 0)
                {
                    return;
                }
                DetailMode = 0;

                DeleteAppBarButton.Visibility = Visibility.Visible;

                DetailsCreateButton.Content = "Change";
                DetailsCreateButton.Click -= DetailsCreateButton_Click;
                DetailsCreateButton.Click += DetailsChangeButton_Click;

                DetailsCancelButton.Click -= DetailsCancelButton_Click;
                DetailsCancelButton.Click += DetailsRestoreButton_Click;
            }
            else
            {
                if (DetailMode == 1)
                {
                    return;
                }
                DetailMode = 1;

                DeleteAppBarButton.Visibility = Visibility.Collapsed;

                DetailsCreateButton.Content = "Create";
                
                DetailsCreateButton.Click -= DetailsChangeButton_Click;
                DetailsCreateButton.Click += DetailsCreateButton_Click;

                DetailsCancelButton.Click -= DetailsRestoreButton_Click;
                DetailsCancelButton.Click += DetailsCancelButton_Click;
            }
        }

        private void DetailsRestoreButton_Click(object sender, RoutedEventArgs e)
        {
            SetDetailInfomation(0);
        }

        private async void DetailsChangeButton_Click(object sender, RoutedEventArgs e)
        {
            var a = sender.ToString();
            MessageDialog messageDialog = new MessageDialog("Confirm to change", "Confirm");
           
            messageDialog.Commands.Add(new UICommand("确定", cmd => { }, commandId: 0));
            messageDialog.Commands.Add(new UICommand("取消", cmd => { }, commandId: 1));

            //设置默认按钮，不设置的话默认的确认按钮是第一个按钮
            messageDialog.DefaultCommandIndex = 0;
            messageDialog.CancelCommandIndex = 1;

            var result = await messageDialog.ShowAsync();
            
            if ((int)result.Id == 1)
            {
                return;
            }
            else
            {
                VM.UpdateItem(SelectedItem.ID, DetailsTitleTextBox.Text, DetailsDescriptionTextBox.Text, DueDatePicker.Date, ImageUrl);                
            }
        }

        /// <summary>
        /// 如果反复点击同一元素可以刷新显示
        /// </summary>
        private void MainListView_ItemClick(object sender, ItemClickEventArgs e)
        {
            if (e.ClickedItem == SelectedItem)
            {
                SelectedIndex = MainListView.SelectedIndex;
                if (Window.Current.Bounds.Width >= 800)
                {
                    SetDetailInfomation();
                }
                else
                {
                    (Window.Current.Content as Frame).Navigate(typeof(Views.DetailsPage));
                }
            }
        }
        
        private async void DetailsCreateButton_Click(object sender, RoutedEventArgs e)
        {
            string WrongMessage = null;
            if (string.IsNullOrEmpty(DetailsTitleTextBox.Text))
            {
                WrongMessage += "Title cannot be empty.\n";
            }
            if (string.IsNullOrEmpty(DetailsDescriptionTextBox.Text))
            {
                WrongMessage += "Description cannot be empty.\n";
            }
            if (DueDatePicker.Date > DateTimeOffset.Now)
            {
                WrongMessage += "The DDL has already passed.";
            }
            var newitem = new TodoItem(DetailsTitleTextBox.Text, DetailsDescriptionTextBox.Text, DueDatePicker.Date, false, ImageUrl);
            
            if (!string.IsNullOrEmpty( WrongMessage))
            {
                MessageDialog messageDialog = new MessageDialog(WrongMessage, "OUCH!");
                await messageDialog.ShowAsync();
                return;
            }
            if (VM.IsExsisting(newitem))
            {
                WrongMessage = "We think this thing has already exsisted, because the title, the description and the duedate are all same.\n" +
                    "Maybe you should change at least one of them.";
                MessageDialog messageDialog = new MessageDialog(WrongMessage, "OUCH!");
                await messageDialog.ShowAsync();
                return;
            }
            VM.AddNewItem(newitem);
            SetDetailInfomation(1);
        }

        private void DetailsCancelButton_Click(object sender, RoutedEventArgs e)
        {
            SetDetailInfomation();
        }

        private void NewPageAppBarButton_Click(object sender, RoutedEventArgs e)
        {
            if (Window.Current.Bounds.Width < 800)
            {
                var rootFrame = Window.Current.Content as Frame;
                rootFrame.Navigate(typeof(Views.NewPage));
            }
            else
            {
                SetDetailInfomation(1);
                DetailModeChange(ModeChangeArgs.ChangeToCreate);
            }                 
        }

        private async void Button_Upload_Image(object sender, RoutedEventArgs e)
        {
            var fop = new FileOpenPicker();
            fop.ViewMode = PickerViewMode.Thumbnail;
            fop.SuggestedStartLocation = PickerLocationId.PicturesLibrary;
            fop.FileTypeFilter.Add(".jpg");
            fop.FileTypeFilter.Add(".jpeg");
            fop.FileTypeFilter.Add(".png");
            fop.FileTypeFilter.Add(".gif");

            StorageFile file = await fop.PickSingleFileAsync();
            try
            {
                IRandomAccessStream fileStream;
                using (fileStream = await file.OpenAsync(FileAccessMode.Read))
                {
                    BitmapImage bitmapImage = new BitmapImage();
                    await bitmapImage.SetSourceAsync(fileStream);
                    ImageControl.Source = bitmapImage;

                    var name = file.Path.Substring(file.Path.LastIndexOf('\\') + 1);
                    ImageUrl = name;
                    await file.CopyAsync(ApplicationData.Current.LocalFolder, name, NameCollisionOption.ReplaceExisting);
                }
            }
            catch (Exception Ex)
            {
                Debug.WriteLine(Ex.Message);
                return;
            }
        }

        private void AppBarButton_Click(object sender, RoutedEventArgs e)
        {
            VM.RemoveItem((MainListView.SelectedItem as TodoItem).ID);
        }

        public async void DetailSetPic(string ImageUrl)
        {
            if (string.IsNullOrEmpty(ImageUrl))
            {
                ImageControl.Source = new BitmapImage(new Uri("ms-appx:///Assets/icons8-task-50.png", UriKind.Absolute));
            }
            else
            {
                var file = await ApplicationData.Current.LocalFolder.GetFileAsync(ImageUrl);
                IRandomAccessStream fileStream = await file.OpenAsync(FileAccessMode.Read);
                BitmapImage bitmapImage = new BitmapImage();
                await bitmapImage.SetSourceAsync(fileStream);
                ImageControl.Source = bitmapImage;
            }
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            base.OnNavigatedTo(e);
            dataTransferManager.DataRequested += DataRequested;
            if (e.NavigationMode == NavigationMode.New)
            {
                ApplicationData.Current.LocalSettings.Values.Remove("MainPage");
            }
            else
            {
                if (ApplicationData.Current.LocalSettings.Values.ContainsKey("MainPage"))
                {
                    var composite = ApplicationData.Current.LocalSettings.Values["MainPage"] as ApplicationDataCompositeValue;
                    int a = (int)composite["MainPageMode"];
                    if ((int)composite["MainPageMode"] == 0)
                    {
                        SelectedIndex = (int)composite["SelectedIndex"];
                        SelectedItem = VM.Collection[SelectedIndex];                        
                    }
                    DetailsTitleTextBox.Text = composite["Title"] as string;
                    DetailsDescriptionTextBox.Text = composite["Description"] as string;
                    DueDatePicker.Date = (DateTimeOffset)composite["DueDate"];
                    DetailSetPic(composite["ImageUrl"] as string);
                }
            }
        }
        protected override void OnNavigatedFrom(NavigationEventArgs e)
        {
            base.OnNavigatedFrom(e);
            dataTransferManager.DataRequested -= DataRequested;
            if (((App)App.Current).IsSuspend)
            {
                ApplicationDataCompositeValue composite = new ApplicationDataCompositeValue();
                if (DetailMode == 0)
                {
                    composite["MainPageMode"] = 0;
                    composite["SelectedIndex"] = MainListView.SelectedIndex;                    
                }
                else
                {
                    composite["MainPageMode"] = 1;
                }

                composite["Title"] = DetailsTitleTextBox.Text;
                composite["Description"] = DetailsDescriptionTextBox.Text;
                composite["DueDate"] = DueDatePicker.Date;
                composite["ImageUrl"] = ImageUrl;

                ApplicationData.Current.LocalSettings.Values["MainPage"] = composite;
            }
        }
        
        public void UpdatePrimaryTile(object sender, RoutedEventArgs e)
        {
            TileService.UpdatePrimaryTile();
        }

        private string sharetitle, sharedetail, sharedate;
        private StorageFile shareimg;

        private void AutoSuggestBox_TextChanged(AutoSuggestBox sender, AutoSuggestBoxTextChangedEventArgs args)
        {
            List<StringBuilder> ResultOfFilter = new List<StringBuilder>();

            var autosuggestbox = sender as AutoSuggestBox;
            using (var conn = new SQLiteConnection("demo.db"))
            {
                using (var statement = conn.Prepare("SELECT TITLE, DESCRIPTION, DUEDATE " +
                    "FROM ITEMS " +
                    "WHERE TITLE LIKE ? " +
                    "OR DESCRIPTION LIKE ? " +
                    "OR DUEDATE LIKE ?"))
                {
                    statement.Bind(1, "%%" + autosuggestbox.Text + "%%");
                    statement.Bind(2, "%%" + autosuggestbox.Text + "%%");
                    statement.Bind(3, "%%" + autosuggestbox.Text + "%%");
                    while (SQLiteResult.ROW == statement.Step())
                    {
                        StringBuilder stringBuilder = new StringBuilder("");
                        stringBuilder.Append(statement[0] as string);
                        stringBuilder.Append("\n");
                        stringBuilder.Append(statement[1] as string);
                        stringBuilder.Append("\n");
                        stringBuilder.Append((statement[2] as string).Substring(0, (statement[2] as string).IndexOf(" ")) + "\n");
                        ResultOfFilter.Add(stringBuilder);
                    }
                }
            }
            autosuggestbox.ItemsSource = ResultOfFilter;
        }
        
        private async void SearchButton_Click(object sender, RoutedEventArgs e)
        {
            var searchl = SearchAutoBox.Text.Trim();
            
            var splist = searchl.Split('\n');
            
            long selectedId = 0;
            StringBuilder stringBuilder = new StringBuilder();
            using (var conn = new SQLiteConnection("demo.db"))
            {
                using (var statement = conn.Prepare("SELECT ID, TITLE, DESCRIPTION, DUEDATE " +
                    "FROM ITEMS " +
                    "WHERE TITLE LIKE ? "))
                {
                    statement.Bind(1, searchl);
                    while (SQLiteResult.ROW == statement.Step())
                    {
                        selectedId = (long)statement[0];
                        stringBuilder.Append("Title: \t\t" + statement[1] as string);
                        stringBuilder.Append("\nDescription:\t");
                        stringBuilder.Append(statement[2] as string);
                        stringBuilder.Append("\nDue date:\t");
                        stringBuilder.Append((statement[3] as string).Substring(0, (statement[3] as string).IndexOf(" ")));                           
                        break;
                    }
                }
            }
            if (stringBuilder.Equals(new StringBuilder()))
            {
                MessageDialog messageDialog = new MessageDialog("Nothing found", "Sorry");
                await messageDialog.ShowAsync();
            }            
            else
            {
                //MessageDialog messageDialog = new MessageDialog(stringBuilder.ToString(), "Details");
                //await messageDialog.ShowAsync();
                foreach (var item in VM.Collection)
                {
                    if (item.ID == selectedId)
                    {
                        SelectedItem = item;
                        SetDetailInfomation();
                    }
                }
            }
            return;
                
        }

        private async void shareOneItem(object sender, RoutedEventArgs e)
        {
            var dc = (sender as FrameworkElement).DataContext;
            var item = (MainListView.ContainerFromItem(dc) as ListViewItem).Content as TodoItem;
            sharetitle = item.Title;
            sharedetail = item.Description;
            string shareimgname = item.ImageUrl;
            var date = item.DueDate.Date;
            sharedate = "\nDue date: " + date.Year + '-' + date.Month + '-' + date.Day;
            if (string.IsNullOrEmpty(shareimgname))
            {
                shareimg = await Package.Current.InstalledLocation.GetFileAsync("Assets\\icons8-task-50.png");
            }
            else
            {
                shareimg = await ApplicationData.Current.LocalFolder.GetFileAsync(shareimgname);
            }
            DataTransferManager.ShowShareUI();
        }

        private void DataRequested(DataTransferManager sender, DataRequestedEventArgs e)
        {
            Debug.WriteLine(e.Request.ToString());
            DataRequest request = e.Request;
            DataPackage requestData = request.Data;
            requestData.Properties.Title = sharetitle;
            requestData.SetText(sharedetail + sharedate);

            // Because we are making async calls in the DataRequested event handler,
            //  we need to get the deferral first.
            DataRequestDeferral deferral = request.GetDeferral();

            // Make sure we always call Complete on the deferral.
            try
            {
                requestData.SetBitmap(RandomAccessStreamReference.CreateFromFile(shareimg));
            }
            finally
            {
                deferral.Complete();
            }
        }

    }
}
