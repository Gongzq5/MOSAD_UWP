﻿#pragma checksum "C:\Users\Gongzq5\source\repos\TODO\TODO\Views\NewPage.xaml" "{406ea660-64cf-4c82-b6f0-42d48172a799}" "DCF4D18C12A040F7A878C0662E26C62D"
//------------------------------------------------------------------------------
// <auto-generated>
//     This code was generated by a tool.
//
//     Changes to this file may cause incorrect behavior and will be lost if
//     the code is regenerated.
// </auto-generated>
//------------------------------------------------------------------------------

namespace TODO.Views
{
    partial class NewPage : 
        global::Windows.UI.Xaml.Controls.Page, 
        global::Windows.UI.Xaml.Markup.IComponentConnector,
        global::Windows.UI.Xaml.Markup.IComponentConnector2
    {
        /// <summary>
        /// Connect()
        /// </summary>
        [global::System.CodeDom.Compiler.GeneratedCodeAttribute("Microsoft.Windows.UI.Xaml.Build.Tasks"," 14.0.0.0")]
        [global::System.Diagnostics.DebuggerNonUserCodeAttribute()]
        public void Connect(int connectionId, object target)
        {
            switch(connectionId)
            {
            case 1:
                {
                    this.ImageControl = (global::Windows.UI.Xaml.Controls.Image)(target);
                }
                break;
            case 2:
                {
                    global::Windows.UI.Xaml.Controls.AppBarButton element2 = (global::Windows.UI.Xaml.Controls.AppBarButton)(target);
                    #line 29 "..\..\..\Views\NewPage.xaml"
                    ((global::Windows.UI.Xaml.Controls.AppBarButton)element2).Click += this.Button_Upload_Image;
                    #line default
                }
                break;
            case 3:
                {
                    this.DetailsTitleTextBox = (global::Windows.UI.Xaml.Controls.TextBox)(target);
                }
                break;
            case 4:
                {
                    this.DetailsDescriptionTextBox = (global::Windows.UI.Xaml.Controls.TextBox)(target);
                }
                break;
            case 5:
                {
                    this.DueDatePicker = (global::Windows.UI.Xaml.Controls.DatePicker)(target);
                }
                break;
            case 6:
                {
                    this.DetailsCreateButton = (global::Windows.UI.Xaml.Controls.Button)(target);
                    #line 37 "..\..\..\Views\NewPage.xaml"
                    ((global::Windows.UI.Xaml.Controls.Button)this.DetailsCreateButton).Click += this.DetailsCreateButton_Click;
                    #line default
                }
                break;
            case 7:
                {
                    this.DetailsCancelButton = (global::Windows.UI.Xaml.Controls.Button)(target);
                    #line 39 "..\..\..\Views\NewPage.xaml"
                    ((global::Windows.UI.Xaml.Controls.Button)this.DetailsCancelButton).Click += this.DetailsCancelButton_Click;
                    #line default
                }
                break;
            default:
                break;
            }
            this._contentLoaded = true;
        }

        [global::System.CodeDom.Compiler.GeneratedCodeAttribute("Microsoft.Windows.UI.Xaml.Build.Tasks"," 14.0.0.0")]
        [global::System.Diagnostics.DebuggerNonUserCodeAttribute()]
        public global::Windows.UI.Xaml.Markup.IComponentConnector GetBindingConnector(int connectionId, object target)
        {
            global::Windows.UI.Xaml.Markup.IComponentConnector returnValue = null;
            return returnValue;
        }
    }
}

