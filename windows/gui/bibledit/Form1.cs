using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Diagnostics;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Runtime.InteropServices;


namespace Bibledit
{

    public partial class Form1 : Form
    {

        [DllImport("libbibledit.dll")]
        public static extern void bibledit_run();

        public Form1()
        {
            InitializeComponent();
        }


        private void Form1_Load(object sender, EventArgs e)
        {
            feedback("");
            try
            {
                bibledit_run();
            }
            catch (Exception exception)
            {
                feedback (exception.Message);
            }
            setTimezone();
        }


        private void linkLabel_LinkClicked(object sender, LinkLabelLinkClickedEventArgs e)
        {
            Process.Start ("http://localhost:8080");
        }


        private void Form1_FormClosing(object sender, FormClosingEventArgs e)
        {
            try
            {
                bibledit_run();
            }
            catch (Exception exception)
            {
                feedback(exception.Message);
            }
        }


        private void feedback(String message)
        {
            label.Text = message;
        }


        private void setTimezone ()
        {
            // Set the timezone in the Bibledit library.
            TimeZoneInfo localZone = TimeZoneInfo.Local;
            int utcOffsetMinutes = TimeZoneInfo.Local.BaseUtcOffset.Hours * 60 + TimeZoneInfo.Local.BaseUtcOffset.Minutes;
            String uri = "administration/settings.php?utcoffset=" + utcOffsetMinutes.ToString();
        }


    }
}
