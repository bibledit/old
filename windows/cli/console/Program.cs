using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.InteropServices;


namespace console
{
    class Program
    {

        [DllImport("libbibledit.dll", CallingConvention = CallingConvention.Cdecl)] 
        public static extern void bibledit_root_win (string directory);

        [DllImport("libbibledit.dll")] 
        public static extern void bibledit_start_win();

        [DllImport("libbibledit.dll")] 
        public static extern bool bibledit_running_win();

        [DllImport("libbibledit.dll")] 
        public static extern void bibledit_stop_win();

        static void Main(string[] args)
        {
            String exepath = System.Reflection.Assembly.GetEntryAssembly().Location;
            String directory = System.IO.Path.GetDirectoryName(exepath);
            bibledit_root_win(directory.ToString());
            bibledit_start_win();
            while (bibledit_running_win()) ;
        }
    }
}
