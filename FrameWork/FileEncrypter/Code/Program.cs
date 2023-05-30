using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.IO;
using System.Diagnostics;

namespace FileEncrypter.Code
{
	public struct Project
	{
		public string m_sName { get; set; }
		public string m_sDataPath { get; set; }
		public string m_sPackedDataPath { get; set; }

		public bool IsValid()
		{
			if( m_sName == null || m_sName.Length <= 0 || m_sDataPath == null || m_sDataPath.Length <= 0 )
				return false;

			if( m_sPackedDataPath == null || m_sPackedDataPath.Length <= 0 )
				return false;

			return true;
		}
	}
	static class Program
	{
		public static string s_sSettingsFolder { get; private set; }

		public static string GetFileOrDirectoryName( string _sPath )
		{
			int iStartFileName = _sPath.LastIndexOf( "\\" ) + 1;

			if( iStartFileName < 0 )
				return _sPath;

			return _sPath.Substring( iStartFileName );
		}

		/// <summary>
		///  The main entry point for the application.
		/// </summary>
		[STAThread]
		static void Main()
		{
			Application.SetHighDpiMode( HighDpiMode.SystemAware );
			Application.EnableVisualStyles();
			Application.SetCompatibleTextRenderingDefault( false );

			s_sSettingsFolder = Environment.GetFolderPath( Environment.SpecialFolder.MyDocuments );

			if( s_sSettingsFolder.Length > 0 )
				s_sSettingsFolder += "\\FaZoN\\FileEncrypter\\";

			Directory.CreateDirectory( s_sSettingsFolder );

			ProjectSelector form = new ProjectSelector();
			//form.SetFocus();
			if( form.ShowDialog() == DialogResult.OK )
			{
				Application.Run( new MainWindow() );
			}
			//Application.Run( new ProjectSelector() );
		}
	}
}
