using System;

namespace asm
{
	class MainClass
	{
		// -----------------------------------------------------------------------------
		// Memory definitions

		static String aString = 
			"\n" + "mvi a,3"
			+ "\n" + "mvi b,5";

		public static void Main(string[] args)
		{
			Console.WriteLine("Program code:" + aString);
		}
	}
}
