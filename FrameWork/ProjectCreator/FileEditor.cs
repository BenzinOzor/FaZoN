using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ProjectCreator
{
	public static partial class FileEditor
	{
		static public string GetFileContent(string _sFilePath)
		{
			StreamReader oReader = new StreamReader(File.OpenRead(_sFilePath));

			string sFileContent = oReader.ReadToEnd();

			oReader.Close();

			return sFileContent;
		}

		static public void SetFileNewContent(string _sFilePath, string _sFileNewContent)
		{
			FileStream oFile = new FileStream(_sFilePath, FileMode.Create);
			StreamWriter writer = new StreamWriter(oFile);

			writer.Write(_sFileNewContent);

			writer.Close();
		}

		static public void EditMain()
		{
			Form1 form1 = (Form1)Application.OpenForms["Form1"];

			string sFilePath = $"{form1.m_sProjectRoot}\\Code\\{form1.TxtBx_SourcesDirectoryName.Text}\\main.cpp";

			form1.FileEditionLog(sFilePath);

			string sFileContent = GetFileContent(sFilePath);

			sFileContent = sFileContent.Replace("[Date]", DateTime.Today.ToString("dd/MM/yyyy"));
			sFileContent = sFileContent.Replace("[ProjectName]", $"{form1.TxtBx_ProjectName.Text}");
			sFileContent = sFileContent.Replace("[ProjectType]", $"{form1.comboBox1.Text}");

			SetFileNewContent(sFilePath, sFileContent);
		}

		static public void EditVcxproj()
		{
			Form1 form1 = (Form1)Application.OpenForms["Form1"];

			string sFilePath = $"{form1.m_sProjectRoot}\\Code\\{form1.TxtBx_ProjectName.Text}.vcxproj";

			form1.FileEditionLog(sFilePath);

			string sFileContent = GetFileContent(sFilePath);

			sFileContent = sFileContent.Replace("[ProjectName]", $"{form1.TxtBx_ProjectName.Text}");
			sFileContent = sFileContent.Replace("[ProjectType]", $"{form1.Combo_ProjectTypes.SelectedItem}");
			sFileContent = sFileContent.Replace("[SourcesDirectory]", $"{form1.TxtBx_SourcesDirectoryName.Text}");

			SetFileNewContent(sFilePath, sFileContent);

			sFileContent = GetFileContent(sFilePath);
			sFileContent = sFileContent.Replace("[ProjectExtension]", form1.GetProjectFileExtension());
			SetFileNewContent(sFilePath, sFileContent);
		}

		static public void EditVcxprojFilters()
		{
			Form1 form1 = (Form1)Application.OpenForms["Form1"];

			string sFilePath = $"{form1.m_sProjectRoot}\\Code\\{form1.TxtBx_ProjectName.Text}.vcxproj.filters";

			form1.FileEditionLog(sFilePath);

			string sFileContent = GetFileContent(sFilePath);

			sFileContent = sFileContent.Replace("[SourcesDirectory]", $"{form1.TxtBx_SourcesDirectoryName.Text}");

			SetFileNewContent(sFilePath, sFileContent);
		}

		static public string GetDebuggerCommand( Form1 _rForm )
		{
			if (_rForm.GetProjectFileExtension() == ".dll" )
				return "\r\n    <LocalDebuggerCommand>$(SolutionDir)FrameWork\\DllLauncher\\Bin\\$(Configuration)\\DllLauncher.exe</LocalDebuggerCommand>\r\n    <LocalDebuggerCommandArguments>\"$(TargetPath)\"</LocalDebuggerCommandArguments>";

			return "";
		}

		static public void EditVcxprojUser()
		{
			Form1 form1 = (Form1)Application.OpenForms["Form1"];

			string sFilePath = $"{form1.m_sProjectRoot}\\Code\\{form1.TxtBx_ProjectName.Text}.vcxproj.user";

			form1.FileEditionLog(sFilePath);

			string sFileContent = GetFileContent(sFilePath);

			sFileContent = sFileContent.Replace("[DebuggerCommand]", GetDebuggerCommand( form1 ));

			SetFileNewContent(sFilePath, sFileContent);
		}
	}
}
