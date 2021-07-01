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
			const float tempGrad = 0.0065f;
			const float specificR = 287.053f;
			const float g = 9.80665f;

			float[] testTimes = new float[]{
				173.0039978f,
				173.0209961f,
				173.0390015f,
				173.0559998f,
				173.0740051f,
				173.0910034f,
				173.1080017f,
				173.1260071f,
				173.1419983f,
				173.1600037f,
				173.177002f,
				173.1950073f,
				173.2120056f,
				173.2290039f,
				173.2460022f,
				173.2630005f,
				173.2810059f,
				173.2980042f,
				173.3150024f,
				173.3329926f,
				173.348999f,
				173.3670044f,
				173.3840027f,
				173.401001f,
				173.4190063f,
				173.4360046f,
				173.4539948f,
				173.470993f,
				173.4889984f,
				173.5059967f,
				173.522995f,
			};
			float[] testPressures = new float[]{
				82577.09375f,
				82578.04688f,
				82576.75781f,
				82570.57031f,
				82572,
				82576.6875f,
				82579.875f,
				82575.125f,
				82570.64844f,
				82570.10938f,
				82574.72656f,
				82570.25f,
				82571.67188f,
				82573.03125f,
				82570.25f,
				82574.25781f,
				82573.51563f,
				82568.55469f,
				82571.13281f,
				82572.49219f,
				82573.71875f,
				82573.03125f,
				82572.89844f,
				82572.49219f,
				82568.55469f,
				82571.27344f,
				82571,
				82570.11719f,
				82570.58594f,
				82571.0625f,
				82570.99219f,
			};

			int DataPoints = 0;
			int slopeCacheValid = 0;
			float slopeCacheValue = 0;
			float sumX = 0;
			float sumY = 0;
			float sumXX = 0;
			float sumXY = 0;

			for (int i = 0; i < testTimes.Length; i++)
			{
				float newY = testTimes[i];
				float newX = (25.33f+273.15f) / tempGrad * (float)(Math.Pow(testPressures[i] / 82216.38f, -tempGrad * specificR / g) - 1);
				sumX += newX;
				sumY += newY;
				sumXX += (newX * newX);
				sumXY += (newX * newY);
				DataPoints += 1;
				slopeCacheValid = 0;
			}

			float partA, partB;

			partA = (DataPoints * sumXY) - (sumX * sumY);
			partB = (DataPoints * sumXX) - (sumX * sumX);
			if (partB == 0.0f)
			{
				slopeCacheValue = 0;
			}
			else
			{
				slopeCacheValue = (partA / partB);
			}

			Console.WriteLine(slopeCacheValue);

			bool saveHeight = false;

			int[] durchschnittVon = { 1 };

			float[] startPressure = { 82216.38f };
			float[] startHeight = { 1800 };
			float[] startTemp = { 25.33f + 273.15f };
			for (int i = 0; i < startPressure.Length; i++)
			{
				float redLuft = startPressure[i] / (float)Math.Pow((1 - tempGrad * startHeight[i] / startTemp[i]), 0.03416f / tempGrad);
				Console.WriteLine(i.ToString() + ". Reduzierter Luftdruck von: " + redLuft);
			}
			string[][] allLines = { File.ReadAllLines(@"..\..\GleitschirmFlug1.txt") };
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
