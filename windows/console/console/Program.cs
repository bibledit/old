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
        
        [DllImport("libbibledit.dll")]
        public static extern void bibledit_run();

        static void Main(string[] args)
        {
            bibledit_run();

        }
    }
}
