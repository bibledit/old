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

        //[DllImport("bibleditlibrarywrapper.dll")]
        //public static extern string bibledit_wrapper_get_version_number();
        Process LibBibledit;


        public Form1()
        {
            InitializeComponent();
        }


        private void Form1_Load(object sender, EventArgs e)
        {
            feedback("");
            try
            {
                //feedback(bibledit_wrapper_get_version_number ());
                LibBibledit = new Process();
                LibBibledit.StartInfo.WorkingDirectory = System.IO.Path.Combine(Application.StartupPath);
                LibBibledit.StartInfo.FileName = "server.exe";
                LibBibledit.StartInfo.Arguments = "";
                LibBibledit.StartInfo.CreateNoWindow = true;
                LibBibledit.StartInfo.WindowStyle = ProcessWindowStyle.Hidden;
                LibBibledit.EnableRaisingEvents = true;
                LibBibledit.Exited += new EventHandler (ProcessExited);
                LibBibledit.Start();
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
                LibBibledit.CloseMainWindow();
                LibBibledit.Kill();
                LibBibledit.WaitForExit();
                LibBibledit.Close();
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


        private void ProcessExited (object sender, System.EventArgs e)
        {
            feedback("exited");
            // myProcess.ExitTime, myProcess.ExitCode
            LibBibledit.Start();
        }

    }
}
