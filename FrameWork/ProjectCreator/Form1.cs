using System.Drawing;
using System;

namespace ProjectCreator
{
	public partial class Form1 : Form
	{
		public string m_sSettingsFolder { get; private set; }
		public string m_sSolutionLocation { get; private set; }
		public string m_sSolutionName { get; private set; }
		public string m_sProjectRoot { get; private set; }

		public string m_sDataFolder { get; private set; }

		public Form1()
		{
			InitializeComponent();

			Combo_ProjectTypes.Items.Clear();
			TxtBx_Logs.Clear();
			Btn_CreateProject.Enabled = false;

			m_sSettingsFolder = Environment.GetFolderPath(Environment.SpecialFolder.MyDocuments);

			if (m_sSettingsFolder.Length > 0)
				m_sSettingsFolder += "\\FaZoN Apps\\Project Creator\\";

			Directory.CreateDirectory(m_sSettingsFolder);

			m_sDataFolder = Directory.GetParent(Environment.CurrentDirectory).Parent.Parent.FullName;
			m_sDataFolder += "\\Data";
		}

		public static string GetFileOrDirectoryName(string _sPath)
		{
			int iStartFileName = _sPath.LastIndexOf("\\") + 1;

			if (iStartFileName < 0)
				return _sPath;

			return _sPath.Substring(iStartFileName);
		}

		public string GetProjectFileExtension()
		{
			if (RdBtn_Dll.Checked)
				return ".dll";
			if (RdBtn_Exe.Checked)
				return ".exe";

			return ".exe";
		}

		public void Log(string _sMessage, Color _oColor, bool _bAddEndOfLine = true)
		{
			TxtBx_Logs.SelectionColor = _oColor;
			TxtBx_Logs.AppendText($"{_sMessage}" + (_bAddEndOfLine ? "\n" : ""));
			TxtBx_Logs.SelectionStart = TxtBx_Logs.Text.Length;
			TxtBx_Logs.ScrollToCaret();
		}

		public void Log(string _sMessage, Color _oColor, Color _oBGColor, bool _bAddEndOfLine = true)
		{
			TxtBx_Logs.SelectionBackColor = _oBGColor;
			TxtBx_Logs.SelectionColor = _oColor;
			TxtBx_Logs.AppendText($"{_sMessage}" + (_bAddEndOfLine ? "\n" : ""));
			TxtBx_Logs.SelectionStart = TxtBx_Logs.Text.Length;
			TxtBx_Logs.ScrollToCaret();
			TxtBx_Logs.SelectionBackColor = Color.White;
		}

		public void DirectoryCreationLog(string _sGrayMessage, string _sBlackMessage)
		{
			Log(_sGrayMessage, Color.Gray, false);
			Log(_sBlackMessage, Color.Black);
		}

		public void CreateDirectoryAndLog(string _sDirectoryPath)
		{
			DirectoryCreationLog("Creating directory at: ", $"{_sDirectoryPath}");
			Directory.CreateDirectory($"{_sDirectoryPath}");
		}

		public void CopyFileAndLog(string _sFileName, string _sDestFolder, string _sFileNewName = "")
		{
			if (_sFileNewName.Length == 0)
				_sFileNewName = _sFileName;

			if (File.Exists($"{m_sDataFolder}\\{_sFileName}") == false)
			{
				Log($"{_sFileName} doesn't exist, ignoring copy.", Color.Red);
				return;
			}

			Log("Copying ", Color.Gray, false);
			Log($"{_sFileName} ", Color.Black, false);
			Log("to: ", Color.Gray, false);
			Log($"{m_sProjectRoot}{_sDestFolder}", Color.Black);


			File.Copy($"{m_sDataFolder}\\{_sFileName}", $"{m_sProjectRoot}{_sDestFolder}\\{_sFileNewName}", true);
		}

		public void FileEditionLog(string _sFileName)
		{
			Log("Editing ", Color.Gray, false);
			Log($"{_sFileName} ", Color.Black);
		}

		private void _ScanSolutionFolders()
		{
			Log("Scanning solution folders...", Color.Gray);

			IEnumerable<string> aDirectories = Directory.EnumerateDirectories(m_sSolutionLocation);

			foreach (string sDirectory in aDirectories)
			{
				string sDirectoryName = GetFileOrDirectoryName(sDirectory);

				if (sDirectoryName[0] == '.' || sDirectoryName == "JSON")
					continue;

				Log("Project type found: ", Color.Gray, false);
				Log($"{sDirectoryName}", Color.Black);

				Combo_ProjectTypes.Items.Add(sDirectoryName);
			}

			if (Combo_ProjectTypes.Items.Count > 0)
			{
				Combo_ProjectTypes.SelectedItem = Combo_ProjectTypes.Items[0];
				Btn_CreateProject.Enabled = true;
			}
		}

		private void _CreateDirectories()
		{
			Log("=====================DIRECTORIES CREATION=====================", Color.LightSeaGreen);
			DirectoryCreationLog("Creating directories at: ", $"{m_sSolutionLocation}{Combo_ProjectTypes.SelectedItem}");

			m_sProjectRoot = $"{m_sSolutionLocation}{Combo_ProjectTypes.SelectedItem}\\{TxtBx_ProjectName.Text}";
			CreateDirectoryAndLog($"{m_sProjectRoot}");

			CreateDirectoryAndLog($"{m_sProjectRoot}\\Code\\{TxtBx_SourcesDirectoryName.Text}");
			CreateDirectoryAndLog($"{m_sProjectRoot}\\Data\\Audio");
			CreateDirectoryAndLog($"{m_sProjectRoot}\\Data\\Display");
			CreateDirectoryAndLog($"{m_sProjectRoot}\\Data\\Misc");
			CreateDirectoryAndLog($"{m_sProjectRoot}\\Data\\XMLFiles");
		}

		private void _CopyFiles()
		{
			Log("=========================FILES COPY=========================", Color.LightSkyBlue);
			CopyFileAndLog("Project.vcxproj", "\\Code", $"{TxtBx_ProjectName.Text}.vcxproj");
			CopyFileAndLog("Project.vcxproj.filters", "\\Code", $"{TxtBx_ProjectName.Text}.vcxproj.filters");
			CopyFileAndLog("Project.vcxproj.user", "\\Code", $"{TxtBx_ProjectName.Text}.vcxproj.user");
			CopyFileAndLog("main.cpp", $"\\Code\\{TxtBx_SourcesDirectoryName.Text}");
			CopyFileAndLog("res.aps", "\\Data");
			CopyFileAndLog("res.rc", "\\Data");
			CopyFileAndLog("FaZoN.ico", "\\Data\\Misc");
			CopyFileAndLog("fzn.png", "\\Data\\Misc");
			CopyFileAndLog("actionKeys.xml", "\\Data\\XMLFiles");
			CopyFileAndLog("Resources", "\\Data\\XMLFiles");
		}

		private void _EditFiles()
		{
			Log("=======================FILES EDITION=========================", Color.LightCoral);
			FileEditor.EditMain();
			FileEditor.EditVcxproj();
			FileEditor.EditVcxprojUser();
			FileEditor.EditVcxprojFilters();
		}

		private void Btn_LocateSln_Click(object sender, EventArgs e)
		{
			OpenFileDialog openFileDialog = new OpenFileDialog();

			if (openFileDialog.ShowDialog() == DialogResult.OK)
			{
				m_sSolutionLocation = openFileDialog.FileName;

				m_sSolutionName = GetFileOrDirectoryName(m_sSolutionLocation);
				m_sSolutionLocation = m_sSolutionLocation.Substring(0, m_sSolutionLocation.Length - m_sSolutionName.Length);

				TxtBx_LastSolutionLocation.Text = openFileDialog.FileName;

				Combo_ProjectTypes.Items.Clear();
				TxtBx_Logs.Clear();

				Log("=======================SOLUTION SCAN=======================", Color.Plum);
				Log("Solution path: ", Color.Gray, false);
				Log(openFileDialog.FileName, Color.Black);

				_ScanSolutionFolders();
			}
		}

		private void Btn_CreateProject_Click(object sender, EventArgs e)
		{
			_CreateDirectories();
			_CopyFiles();
			_EditFiles();

			Log("									          ", Color.Black, Color.Black);
			Log("=================PROJECT SUCCESSFULLY CREATED==================", Color.LawnGreen, Color.Black);
			Log("									          ", Color.Black, Color.Black);
		}

		private void Btn_Close_Click(object sender, EventArgs e)
		{
			Application.Exit();
		}
	}
}