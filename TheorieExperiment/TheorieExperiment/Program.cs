using System;
using System.Collections.Generic;
using Excel = Microsoft.Office.Interop.Excel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace TheorieExperiment
{
	class Program
	{
		static void Main(string[] args)
		{
			const double std_a = 0.075;
			const double std_h = 0.5;
			const int n = 300;
			const double dT = 0.05;
			//double[] simAcc = new double[n];
			//double[] simVel = new double[n];
			//double[] simHei = new double[n];
			//double[] measAcc = new double[n];
			//double[] measHei = new double[n];
			double v = 0;
			double h = 0;
			double a = 0;
			Random rand = new Random();
			List<Tuple<float, float, float, float, float, float>> allValues = new List<Tuple<float, float, float, float, float, float>>(); //Zeit //SimHei //SimVel //SimAcce //measHei //measAcce
			for (int i = 0; i < n; i++)
			{
				a = 0.3 * Math.Sin(Math.PI * (double)((double)i/(double)n)*4);
				//simAcc[i] = a;
				v += a * dT;
				//simVel[i] = v;
				h += v * dT + (a * dT * dT)/2;
				//simHei[i] = h;
				double measAcceleration = randomGauss(rand ,a, std_a);
				//double measAcceleration = a;
				double measHeight = randomGauss(rand, h, std_h);
				allValues.Add(new Tuple<float, float, float, float, float, float>((float)i* (float)dT, (float)h, (float)v, (float)a, (float)measHeight, (float)measAcceleration));
			}

			System.Globalization.NumberFormatInfo nfi = new System.Globalization.NumberFormatInfo();
			nfi.NumberDecimalSeparator = ".";
			//Zeiten ausgeben um in Arduino zu kopieren
			Console.Write("double times[" + allValues.Count.ToString() + "] = { ");
			for (int i = 0; i < allValues.Count; i++)
			{
				Console.Write(allValues[i].Item1.ToString(nfi) + ", ");
			}
			Console.WriteLine(" };");

			Console.Write("double measHeights[" + allValues.Count.ToString() + "] = { ");
			for (int i = 0; i < allValues.Count; i++)
			{
				Console.Write(allValues[i].Item5.ToString(nfi) + ", ");
			}
			Console.WriteLine(" };");

			Console.Write("double measAccelerations[" + allValues.Count.ToString() + "] = { ");
			for (int i = 0; i < allValues.Count; i++)
			{
				Console.Write(allValues[i].Item6.ToString(nfi) + ", ");
			}
			Console.WriteLine(" };");
			WriteToExcel(allValues);
			Console.ReadKey();
		}

		static double randomGauss(Random rand, double mean, double stdDev)
		{
			//https://stackoverflow.com/questions/218060/random-gaussian-variables
			double u1 = 1.0 - rand.NextDouble(); //uniform(0,1] random doubles
			double u2 = 1.0 - rand.NextDouble();
			double randStdNormal = Math.Sqrt(-2.0 * Math.Log(u1)) * Math.Sin(2.0 * Math.PI * u2); //random normal(0,1)
			double randomGauss = mean + stdDev * randStdNormal; //random normal(mean,stdDev^2)
			return randomGauss;
		}

		public static void WriteToExcel(List<Tuple<float, float, float, float, float, float>> values1)
		{
			Excel.Application excelApp = new Excel.Application();
			if (excelApp != null)
			{
				Excel.Workbook excelWorkbook = excelApp.Workbooks.Add();
				Excel.Worksheet excelWorksheet = (Excel.Worksheet)excelWorkbook.Sheets.Add();
				excelWorksheet.Cells[1, 1] = "Zeit";
				excelWorksheet.Cells[1, 2] = "SimHei";
				excelWorksheet.Cells[1, 3] = "SimVel";
				excelWorksheet.Cells[1, 4] = "SimAcc";
				excelWorksheet.Cells[1, 5] = "MeasHei";
				excelWorksheet.Cells[1, 6] = "MeasAcc";
				for (int y = 0; y < values1.Count; y++)
				{
					excelWorksheet.Cells[y + 3, 1] = values1[y].Item1;
					excelWorksheet.Cells[y + 3, 2] = values1[y].Item2;
					excelWorksheet.Cells[y + 3, 3] = values1[y].Item3;
					excelWorksheet.Cells[y + 3, 4] = values1[y].Item4;
					excelWorksheet.Cells[y + 3, 5] = values1[y].Item5;
					excelWorksheet.Cells[y + 3, 6] = values1[y].Item6;
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
