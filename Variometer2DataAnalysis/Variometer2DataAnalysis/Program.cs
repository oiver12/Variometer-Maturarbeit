using System;
using System.Collections;
using System.Collections.Generic;
using System.Globalization;
using System.IO;
using Excel = Microsoft.Office.Interop.Excel;

namespace Variometer2DataAnalysis
{
	class Program
	{
		static void Main(string[] args)
		{
			float startTemp = 22.86f + 273.15f;
			string[] allLines = File.ReadAllLines(@"..\..\FlugKalmanFilter.txt");
			var allValues = ReadFile(allLines, startTemp, 1);
			getProcessNoiseFlight(allValues);
			WriteToExcel(allValues);
			//0,110104126357015 0,00264171387702353
			//0,129773915112849 0,00691318693156789
			/*int gleitendAnzahl = 1;
			var messung1 = ReadFile(File.ReadAllLines(@"..\..\Messung5Min1.txt"), 27.43f + 273.15f, gleitendAnzahl);
			var messung2 = ReadFile(File.ReadAllLines(@"..\..\Messung5Min2.txt"), 26.89f + 273.15f, gleitendAnzahl);
			var messung3 = ReadFile(File.ReadAllLines(@"..\..\Messung5Min3.txt"), 24.96f + 273.15f, gleitendAnzahl);
			var messung4 = ReadFile(File.ReadAllLines(@"..\..\Messung5Min4.txt"), 25.68f + 273.15f, gleitendAnzahl);
			var messung5 = ReadFile(File.ReadAllLines(@"..\..\Messung5Min5.txt"), 26.06f + 273.15f, gleitendAnzahl);
			var messung6 = ReadFile(File.ReadAllLines(@"..\..\Messung5Min6.txt"), 26.14f + 273.15f, gleitendAnzahl);
			var messung7 = ReadFile(File.ReadAllLines(@"..\..\Messung5Min7.txt"), 26.33f + 273.15f, gleitendAnzahl);
			var messung8 = ReadFile(File.ReadAllLines(@"..\..\Messung5Min8.txt"), 25.10f + 273.15f, gleitendAnzahl);
			var messung9 = ReadFile(File.ReadAllLines(@"..\..\Messung5Min9.txt"), 26.13f + 273.15f, gleitendAnzahl);
			var messung10 = ReadFile(File.ReadAllLines(@"..\..\Messung5Min10.txt"), 25.35f + 273.15f, gleitendAnzahl);
			getStandard(messung1, messung2, messung3, messung4, messung5, messung6, messung7, messung8, messung9, messung10);
			WriteToExcel(messung1);*/
			Console.ReadKey();
		}

		public static List<Tuple<float, float, float, float>> ReadFile(string[] allLines, float startTemp, int gleitendAnzahl)
		{
			const float tempGrad = 0.0065f;
			const float specificR = 287.053f;
			const float g = 9.80665f;

			int countGleitend = 0;
			double sumGleitendHeight = 0;
			double sumGleitendAcc = 0;
			float startPressure = 0;
			List<Tuple<float, float, float, float>> allValues = new List<Tuple<float, float, float, float>>();
			List<float> temps = new List<float>();
			Queue<float> fifoPressure = new Queue<float>();
			Queue<float> fifoAcc = new Queue<float>();
			float time = 0;
			for (int y = 1; y < allLines.Length; y++)
			{
				if (allLines[y][0] == 'F')
				{
					temps.Add(float.Parse(allLines[y].Substring(1, allLines[y].Length - 2), CultureInfo.InvariantCulture));
					continue;
				}
				int indexBracket = allLines[y].IndexOf(';');
				int indexNewBracket = allLines[y].IndexOf(';', indexBracket + 1);
				float deltaTime = float.Parse(allLines[y].Substring(0, indexBracket), CultureInfo.InvariantCulture);
				time += deltaTime;
				float Pressure = float.Parse(allLines[y].Substring(indexBracket + 1, indexNewBracket - indexBracket - 1), CultureInfo.InvariantCulture);
				if (y == 1)
				{
					time = 0;
					startPressure = Pressure;
				}
				double height = startTemp / tempGrad * (Math.Pow(Pressure / startPressure, -tempGrad * specificR / g) - 1);
				indexBracket = indexNewBracket;
				indexNewBracket = allLines[y].IndexOf(';', indexBracket + 1);
				float acc = float.Parse(allLines[y].Substring(indexBracket + 1, indexNewBracket - indexBracket - 1), CultureInfo.InvariantCulture);
				indexBracket = indexNewBracket;
				indexNewBracket = allLines[y].IndexOf(';', indexBracket + 1);
				float vel = float.Parse(allLines[y].Substring(indexBracket + 1, indexNewBracket - indexBracket - 1), CultureInfo.InvariantCulture);
				indexBracket = indexNewBracket;
				indexNewBracket = allLines[y].IndexOf(';', indexBracket + 1);
				float quatW = float.Parse(allLines[y].Substring(indexBracket + 1, indexNewBracket - indexBracket - 1), CultureInfo.InvariantCulture);
				indexBracket = indexNewBracket;
				indexNewBracket = allLines[y].IndexOf(';', indexBracket + 1);
				float quatX = float.Parse(allLines[y].Substring(indexBracket + 1, indexNewBracket - indexBracket - 1), CultureInfo.InvariantCulture);
				indexBracket = indexNewBracket;
				indexNewBracket = allLines[y].IndexOf(';', indexBracket + 1);
				float quatY = float.Parse(allLines[y].Substring(indexBracket + 1, indexNewBracket - indexBracket - 1), CultureInfo.InvariantCulture);
				indexBracket = indexNewBracket;
				float quatZ = float.Parse(allLines[y].Substring(indexBracket + 1, allLines[y].Length - indexBracket - 1), CultureInfo.InvariantCulture);
				if(gleitendAnzahl == 1)
				{
					allValues.Add(new Tuple<float, float, float, float>(time, (float)height, acc, vel));
					continue;
				}
				countGleitend++;
				sumGleitendHeight += height;
				sumGleitendAcc += acc;
				if(countGleitend >= gleitendAnzahl)
				{
					allValues.Add(new Tuple<float, float, float, float>(time, (float)sumGleitendHeight/(float)gleitendAnzahl, (float)sumGleitendAcc/(float)gleitendAnzahl, vel));
					sumGleitendHeight -= fifoPressure.Dequeue();
					sumGleitendAcc -= fifoAcc.Dequeue();
					countGleitend--;
				}
				fifoPressure.Enqueue((float)height);
				fifoAcc.Enqueue(acc);
			}
			return allValues;
		}

		public static void getProcessNoiseFlight(List<Tuple<float, float, float, float>> allValues)
		{
			float startTime = 30.0f; //sekunden seit Aufzeichnungsbeginn
			float endTime = 800.0f;
			double medAcc = 0;
			float heighest = 0;
			for (int i = 1; i < allValues.Count; i++)
			{
				if (allValues[i].Item1 / 1000.0 < startTime || allValues[i].Item1 / 1000.0 > endTime)
				{
					continue;
				}
				medAcc += allValues[i].Item3;
				if (Math.Abs(allValues[i].Item3) > Math.Abs(heighest))
					heighest = allValues[i].Item3;
			}
			medAcc /= allValues.Count;
			double standDvAcc = 0;
			for (int i = 0; i < allValues.Count; i++)
			{
				if (allValues[i].Item1 / 1000.0 < startTime || allValues[i].Item1 / 1000.0 > endTime)
				{
					continue;
				}
				standDvAcc += (allValues[i].Item3 - medAcc) * (allValues[i].Item3 - medAcc);
			}
			Console.WriteLine(Math.Sqrt(standDvAcc / ((allValues.Count))));
		}

		public static void getStandard(params List<Tuple<float, float, float, float>>[] allValues)
		{
			double[] medPressure = new double[allValues.Length];
			double[] medAcc = new double[allValues.Length];
			for (int i = 0; i < allValues.Length; i++)
			{
				medPressure[i] = 0;
				medAcc[i] = 0;
				for (int y = 0; y < allValues[i].Count; y++)
				{
					medPressure[i] += allValues[i][y].Item2;
					medAcc[i] += allValues[i][y].Item3;
				}
				medPressure[i] /= allValues[i].Count;
				medAcc[i] /= allValues[i].Count;
			}
			double standDvPressure = 0;
			double standDvAcc = 0;
			int count = 0;
			for (int i = 0; i < allValues.Length; i++)
			{
				for (int y = 0; y < allValues[i].Count; y++)
				{
					count++;
					standDvPressure += (allValues[i][y].Item2 - medPressure[i]) * (allValues[i][y].Item2 - medPressure[i]);
					standDvAcc += (allValues[i][y].Item3 - medAcc[i]) * (allValues[i][y].Item3 - medAcc[i]);
				}
			}
			standDvPressure /= count;
			standDvAcc /= count;
			Console.WriteLine(Math.Sqrt(standDvPressure).ToString() + " " + Math.Sqrt(standDvAcc).ToString());
			double mde = 0;
			double meda = 0;
			for (int i = 0; i < medPressure.Length; i++)
			{
				mde += medPressure[i];
				meda += medAcc[i];
			}
			Console.WriteLine((mde/medPressure.Length) + "  " + (meda / medPressure.Length));
		}

		public static void WriteToExcel(List<Tuple<float, float, float, float>> values1)
		{
			Console.WriteLine("Here");
			Excel.Application excelApp = new Excel.Application();
			if (excelApp != null)
			{
				Excel._Workbook excelWorkbook = excelApp.Workbooks.Add();
				Excel._Worksheet excelWorksheet = (Excel.Worksheet)excelWorkbook.ActiveSheet;
				for (int y = 0; y < values1.Count; y++)
				{
					excelWorksheet.Cells[y + 3, 1] = values1[y].Item1 / 1000f;
					excelWorksheet.Cells[y + 3, 2] = values1[y].Item2;
					excelWorksheet.Cells[y + 3, 3] = values1[y].Item3;
					excelWorksheet.Cells[y + 3, 4] = values1[y].Item4;
				}
				Console.WriteLine("Finished");
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
