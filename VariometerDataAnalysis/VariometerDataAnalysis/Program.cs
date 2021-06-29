using System;
using System.Collections.Generic;
using System.IO;
using Excel = Microsoft.Office.Interop.Excel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace VariometerDataAnalysis
{
	class Program
	{
		static void Main(string[] args)
		{
			bool saveHeight = false;

			int[] durchschnittVon = { 1, 1 };

			const float tempGrad = 0.0065f;
			const float specificR = 287.053f;
			const float g = 9.80665f;

			float[] startPressure = { 97617.02f, 97616.01f};
			float[] startHeight = { 320, 320};
			float[] startTemp = { 24.87f + 273.15f, 26.55f + 273.15f };
			for (int i = 0; i < startPressure.Length; i++)
			{
				float redLuft = startPressure[i] / (float)Math.Pow((1 - tempGrad * startHeight[i] / startTemp[i]), 0.03416f / tempGrad);
				Console.WriteLine(i.ToString() + ". Reduzierter Luftdruck von: " + redLuft);
			}
			string[][] allLines = { File.ReadAllLines(@"..\..\KühlschrankOhneTempMessung.txt"), File.ReadAllLines(@"..\..\KühlschrankMitTempMessung.txt") };
			float[] time = new float[startPressure.Length];
			float[] pressure = new float[startPressure.Length];
			for (int i = 0; i < pressure.Length; i++)
			{
				pressure[i] = startPressure[i];
			}
			List<Tuple<float, float>>[] allValues = new List<Tuple<float, float>>[pressure.Length];
			for (int i = 0; i < allValues.Length; i++)
			{
				allValues[i] = new List<Tuple<float, float>>();
			}
			float[] summePressure = new float[startPressure.Length];
			float[] summeZeit = new float[startPressure.Length];
			for (int i = 0; i < allLines.Length; i++)
			{
				for (int y = 1; y < allLines[i].Length; y++)
				{
					int indexBracket = allLines[i][y].IndexOf(';');
					float deltaTime = float.Parse(allLines[i][y].Substring(0, indexBracket).Replace('.', ','));
					float deltaPressure = float.Parse(allLines[i][y].Substring(indexBracket + 1, allLines[i][y].Length - indexBracket - 1).Replace('.', ','));
					time[i] += deltaTime;
					pressure[i] += deltaPressure;
					if (y % durchschnittVon[i] == 0)
					{
						summePressure[i] += pressure[i];
						summeZeit[i] += time[i];
						float heightOrPressure = summePressure[i] / durchschnittVon[i];
						if(saveHeight)
							heightOrPressure = startTemp[i] / tempGrad * (float)(Math.Pow((summePressure[i] / durchschnittVon[i]) / startPressure[i], -tempGrad * specificR / g) - 1);

						allValues[i].Add(new Tuple<float, float>(summeZeit[i] / durchschnittVon[i], heightOrPressure));
						summePressure[i] = 0;
						summeZeit[i] = 0;
					}
					else
					{
						summePressure[i] += pressure[i];
						summeZeit[i] += time[i];
					}
				}
			}
			Console.WriteLine("Finish Berechnen");
			WriteToExcel(allValues);
			Console.WriteLine("All Finish");
			Console.ReadKey();
		}

		public static void WriteToExcel(List<Tuple<float, float>>[] values1)
		{
			Excel.Application excelApp = new Excel.Application();
			if (excelApp != null)
			{
				Excel.Workbook excelWorkbook = excelApp.Workbooks.Add();
				Excel.Worksheet excelWorksheet = (Excel.Worksheet)excelWorkbook.Sheets.Add();
				//excelWorksheet.Cells[1, 1] = "1";
				for (int i = 0; i < values1.Length; i++)
				{
					for (int y = 0; y < values1[i].Count; y++)
					{
						excelWorksheet.Cells[y + 3, i*2+1] = values1[i][y].Item1 / 1000f;
						excelWorksheet.Cells[y + 3, i*2+2] = values1[i][y].Item2;
					}
					Console.WriteLine(i.ToString() + ". Finished");
				}
				excelApp.ActiveWorkbook.SaveAs(@"abc.xlsx");

				excelWorkbook.Close();
				excelApp.Quit();

				System.Runtime.InteropServices.Marshal.FinalReleaseComObject(excelWorksheet);
				System.Runtime.InteropServices.Marshal.FinalReleaseComObject(excelWorkbook);
				System.Runtime.InteropServices.Marshal.FinalReleaseComObject(excelApp);
				GC.Collect();
				GC.WaitForPendingFinalizers();
			}
		}
	}
}
