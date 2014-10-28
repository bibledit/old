using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Diagnostics;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Net;
using System.Threading;

namespace Bibledit
{
   
    public partial class Form1 : Form
    {

        Process LightTPD;
        int scriptCount = 0;


        public Form1()
        {
            InitializeComponent();
        }


        private void Form1_Load(object sender, EventArgs e)
        {
            feedback("");
            LightTPD = new Process();
            try
            {
                LightTPD.StartInfo.WorkingDirectory = System.IO.Path.Combine(Application.StartupPath, "LightTPD");
                LightTPD.StartInfo.FileName = "LightTPD.exe";
                LightTPD.StartInfo.Arguments = "-D";
                LightTPD.StartInfo.CreateNoWindow = true;
                LightTPD.StartInfo.WindowStyle = ProcessWindowStyle.Hidden;
                LightTPD.Start();
            }
            catch (Exception exception)
            {
                feedback (exception.Message);
            }
            setTimezone();
        }


        private void linkLabel_LinkClicked(object sender, LinkLabelLinkClickedEventArgs e)
        {
            Process.Start("http://localhost:54321/index.php");
        }


        private void Form1_FormClosing(object sender, FormClosingEventArgs e)
        {
            try
            {
                LightTPD.CloseMainWindow();
                LightTPD.Kill();
                LightTPD.WaitForExit();
                LightTPD.Close();
            }
            catch (Exception)
            {
            }
        }


        private void timerrun_Tick(object sender, EventArgs e)
        {
            startScript();
        }


        private void timerindex_Tick(object sender, EventArgs e)
        {
            String[] paths = new String[4];
            paths[0] = Application.StartupPath;
            paths[1] = "bibledit-web";
            paths[2] = "timer";
            paths[3] = "index.php";
            String path = System.IO.Path.Combine(paths);
            runScript(path);
        }


        private void startScript () 
        {
            if (scriptCount >= 10) return;
            String folder = System.IO.Path.Combine(Application.StartupPath, "bibledit-web", "processes");
            try {
                String [] files = System.IO.Directory.GetFiles(folder);
                foreach (String file in files) {
                    if (file.Contains(".htaccess")) continue;
                    System.IO.StreamReader reader = new System.IO.StreamReader(file);
                    String contents = reader.ReadToEnd();
                    reader.Close();
                    System.IO.File.Delete(file);
                    String interpreter = contents.Substring(0, 3);
                    if (interpreter != "php") continue;
                    String script = contents.Substring(4);
                    runScript(script);
                    break;
                }
            }
            catch (Exception exception)
            {
                feedback (exception.Message);
			}

        }

        private void runScript (String path)
        {
            Thread thread = new Thread(delegate()
            {
                // One more process will be started.
                scriptCount++;

                Process process = new Process();
                try
                {
                    // To run the scripts properly in php.exe cli, the working directory should be the directory of the script to run.
                    // On Linux this works out of the box, but on Windows, it needs to be set manually.
                    String scriptFolder = path.Replace("\"", "");
                    String[] tokens = scriptFolder.Split(new Char[] { ' ' });
                    scriptFolder = tokens[0];
                    scriptFolder = System.IO.Path.GetDirectoryName(scriptFolder);
                    process.StartInfo.WorkingDirectory = scriptFolder;
                    // The filename to the executable contains the whole path for the executable to be started properly.
                    process.StartInfo.FileName = System.IO.Path.Combine(Application.StartupPath, "php", "php.exe");
                    process.StartInfo.Arguments = path;
                    process.StartInfo.CreateNoWindow = true;
                    process.StartInfo.WindowStyle = ProcessWindowStyle.Hidden;
                    process.StartInfo.UseShellExecute = false;
                    process.StartInfo.RedirectStandardOutput = true;
                    process.StartInfo.RedirectStandardError = true;
                    process.Start();
                    // Wait till the script is ready.
                    process.WaitForExit();
                    // Log the script output, if there was any.
                    // Output indicates errors.
                    String output = process.StandardOutput.ReadToEnd();
                    if (!String.IsNullOrWhiteSpace(output)) logEntry(output);
                    String error = process.StandardError.ReadToEnd();
                    if (!String.IsNullOrWhiteSpace(error)) logEntry(error);
                }
                catch (Exception exception)
                {
                    feedback(exception.Message);
                }

                // Process complete: Decrease process count.
                scriptCount--;
            });
            thread.Start();
        }


        private void fetchScript (String uri, Boolean log)
        {
            Thread thread = new Thread(delegate()
            {
                WebClient client = new WebClient();
                try
                {
                    uri = "http://localhost:54321/" + uri;
                    String output = client.DownloadString(uri);
                    if (log && !String.IsNullOrWhiteSpace(output))
                    {
                        logEntry(uri);
                        logEntry(output);
                     }
                }
                catch (Exception exception)
                {
                    feedback(exception.Message);
                }
            });
            thread.Start();
        }
        

        private void feedback(String message)
        {
            label.Text = message;
        }


        private void logEntry (String message)
        {
            String encoded = System.Uri.EscapeUriString(message);
            String uri = "journal/log.php?msg=" + encoded;
            fetchScript(uri, false);
        }


        private void setTimezone ()
        {
            // Set the timezone in PHP.
            TimeZoneInfo localZone = TimeZoneInfo.Local;
            int utcOffsetMinutes = TimeZoneInfo.Local.BaseUtcOffset.Hours * 60 + TimeZoneInfo.Local.BaseUtcOffset.Minutes;
            String uri = "administration/settings.php?utcoffset=" + utcOffsetMinutes.ToString();
            fetchScript(uri, true);
        }


    }
}
