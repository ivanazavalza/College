/*
 * Program.cs is the starting point for AGMGSK applications.
 * 
 * 
 * Group members:  Jared Fowler  yrolg4tseuq@sbcglobal.net
 * Project 2
 * Spring 2015
 */

using System;

namespace AGMGSKv6
{
#if WINDOWS || LINUX
    static class Program
    {
        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        static void Main(string[] args)
        {
            using (Stage stage = new Stage())
            {
                stage.Run();
            }
        }
    }
#endif
}

