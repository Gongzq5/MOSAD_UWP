﻿using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using TODO.Models;
using TODO.ViewModels;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.Storage;
using Windows.Storage.Pickers;
using Windows.Storage.Streams;
using Windows.UI.Core;
using Windows.UI.Popups;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Media.Imaging;
using Windows.UI.Xaml.Navigation;

// https://go.microsoft.com/fwlink/?LinkId=234238 上介绍了“空白页”项模板

namespace TODO.Views
{
    /// <summary>
    /// 可用于自身或导航至 Frame 内部的空白页。
    /// </summary>
    public sealed partial class DetailsPage : Page
    {
        public TodoItem SelectedItem;
        public int SelectIndex;
        public string ImageUrl;
        public DetailsPage()
        {
            this.InitializeComponent();
            if (MainPage.Current != null)
            {
                SelectIndex = MainPage.Current.SelectedIndex;
                SelectedItem = ItemsManager.Instance.Collection[SelectIndex];
                SetDetailInfomation();
            }
        }

        /// <summary>
        /// 可以反复利用的对Details页面重新置值的操作
        /// </summary>
        public void SetDetailInfomation()
        {
            DetailsTitleTextBox.Text = SelectedItem.Title;
            DetailsDescriptionTextBox.Text = SelectedItem.Description;
            ImageControl.Source = SelectedItem.Image;
        }

        private void DetailsCancelButton_Click(object sender, RoutedEventArgs e)
        {
            SetDetailInfomation();
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

        public async void SetPic(string ImageUrl)
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
            SystemNavigationManager.GetForCurrentView().AppViewBackButtonVisibility = AppViewBackButtonVisibility.Visible;
            if (e.NavigationMode == NavigationMode.New)
            {
                ApplicationData.Current.LocalSettings.Values.Remove("DetailPage");
            }
            else
            {
                if (ApplicationData.Current.LocalSettings.Values.ContainsKey("DetailPage"))
                {
                    var composite = ApplicationData.Current.LocalSettings.Values["DetailPage"] as ApplicationDataCompositeValue;
                    
                    SelectIndex = (int)composite["SelectedIndex"];
                    SelectedItem = ItemsManager.Instance.Collection[SelectIndex];
                    SetDetailInfomation();
                    DetailsTitleTextBox.Text = composite["Title"] as string;
                    DetailsDescriptionTextBox.Text = composite["Description"] as string;
                    DueDatePicker.Date = (DateTimeOffset)composite["DueDate"];
                    SetPic(composite["ImageUrl"] as string);
                }
            }
        }

        protected override void OnNavigatedFrom(NavigationEventArgs e)
        {
            base.OnNavigatedFrom(e);
            bool a = ((App)App.Current).IsSuspend;
            if (((App)App.Current).IsSuspend)
            {
                ApplicationDataCompositeValue composite = new ApplicationDataCompositeValue();

                composite["SelectedIndex"] = SelectIndex;
                composite["Title"] = DetailsTitleTextBox.Text;
                composite["Description"] = DetailsDescriptionTextBox.Text;
                composite["DueDate"] = DueDatePicker.Date;
                composite["ImageUrl"] = ImageUrl;

                ApplicationData.Current.LocalSettings.Values["DetailPage"] = composite;
            }
        }
    }
}
