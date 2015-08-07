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
using System.Net;


namespace Bibledit
{

    public partial class Form1 : Form
    {

        //[DllImport("bibleditlibrarywrapper.dll")]
        //public static extern string bibledit_wrapper_get_version_number();
        Process LibBibledit;
        System.Threading.Timer timer;



        public Form1()
        {
            InitializeComponent();
        }


        private void Form1_Load(object sender, EventArgs e)
        {
            feedback("");
            // Kill any previous servers. This frees the port to connect to.
            foreach (var process in Process.GetProcessesByName("server"))
            {
                process.Kill();
            }
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
                // Set the server to run on one processor only. 
                // That gives a huge boost to the speed of the Cygwin library.
                // The difference in speed is clear: It runs times faster.
                // When a background task runs in Bibledit, the GUI takes a long time to respond without this processor affinity.
                // http://zachsaw.blogspot.nl/2012/10/multithreading-under-cygwin.html
                // http://stackoverflow.com/questions/2510593/how-can-i-set-processor-affinity-in-net
                // What works well too: PsExec.exe -a 1 server.exe
                LibBibledit.ProcessorAffinity = (IntPtr)1;
            }
            catch (Exception exception)
            {
                feedback (exception.Message);
            }
            // Set the timezone with a delay, so it waits till the server is up and running.
            timer = new System.Threading.Timer(obj => { setTimezone(); }, null, 1000, System.Threading.Timeout.Infinite);
        }


        private void linkLabel_LinkClicked(object sender, LinkLabelLinkClickedEventArgs e)
        {
            // This starts the default external web browser.
            // It would have given a better interface to have an embedded browser for Bibledit.
            // But since Microsoft's standard embedded browser component does not well adhere to standard, 
            // sit is better to keep using an external browser.
            Process.Start ("http://localhost:8080");
        }


        private void Form1_FormClosing(object sender, FormClosingEventArgs e)
        {
            try
            {
                LibBibledit.EnableRaisingEvents = false;
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
            try
            {
                // Set the timezone in the Bibledit library.
                TimeZoneInfo localZone = TimeZoneInfo.Local;
                int utcOffsetHours = TimeZoneInfo.Local.BaseUtcOffset.Hours;
                String uri = "http://localhost:8080/administration/timeoffset?offset=" + utcOffsetHours.ToString();
                WebRequest request = WebRequest.Create(uri);
                WebResponse response = request.GetResponse();
                response.Close();
            }
            catch (Exception exception)
            {
                if (exception.Message.Length == 0) { };
            }
        }


        private void ProcessExited (object sender, System.EventArgs e)
        {
            // When the Bibledit library exits or crashea, restart it straightaway.
            LibBibledit.Start();
        }

    }
}
