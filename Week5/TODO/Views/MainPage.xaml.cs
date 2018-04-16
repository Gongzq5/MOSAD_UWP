using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using TODO.Models;
using TODO.Services;
using TODO.ViewModels;
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
        public TodoItem SelectedItem;
        public int SelectedIndex;
        public string ImageUrl;
        public int DetailMode = 1; // 1为new，0为Detail
        
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
                ImageControl.Source = new BitmapImage(new Uri("ms-appx:///Assets/icons8-task-50.png", UriKind.Absolute));
                DetailModeChange(ModeChangeArgs.ChangeToCreate);
            }
            else
            {
                DetailsTitleTextBox.Text = SelectedItem.Title;
                DetailsDescriptionTextBox.Text = SelectedItem.Description;
                ImageControl.Source = SelectedItem.Image;
                DetailModeChange(ModeChangeArgs.CreateToChange);
            }
            
        }

        private void DetailModeChange(ModeChangeArgs e)
        {
            if (e == ModeChangeArgs.CreateToChange)
            {
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
                SelectedItem.Title = DetailsTitleTextBox.Text;
                SelectedItem.Description = DetailsDescriptionTextBox.Text;
                SelectedItem.ImageUrl = ImageUrl;
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
        
        private void DetailsCreateButton_Click(object sender, RoutedEventArgs e)
        {
            VM.AddNewItem(new TodoItem(DetailsTitleTextBox.Text, DetailsDescriptionTextBox.Text, DueDatePicker.Date, false, ImageUrl));
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
            VM.RemoveItem(MainListView.SelectedIndex);
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

        int _count;

        private void UpdateBadge(object sender, RoutedEventArgs e)
        {
            _count++;
            TileService.SetBadgeCountOnTile(_count);
        }

        private void UpdatePrimaryTile(object sender, RoutedEventArgs e)
        {
            var updater = TileUpdateManager.CreateTileUpdaterForApplication();
            updater.Clear();
            updater.EnableNotificationQueue(true);
            for (int i = 0; i < VM.Collection.Count; i++)
            {
                var xmlDoc = TileService.CreateTiles(new PrimaryTile(VM.Collection[i].DueDate.Date.Date.ToString().Substring(0, VM.Collection[i].DueDate.Date.Date.ToString().LastIndexOf(" ")), VM.Collection[i].Title, VM.Collection[i].Description));
                TileNotification notification = new TileNotification(xmlDoc);
                updater.Update(notification);
            }            
        }
    }
}
