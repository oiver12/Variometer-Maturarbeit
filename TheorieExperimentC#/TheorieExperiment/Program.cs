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
			//vom Arduino
			double[] linearRegressionVel = new double[] { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.315213, 0, -0.178152, 0, 0.122478, 0, -0.0870809, 0, -0.0492744, 0, 0.00265718, 0, 0.0253369, 0, 0.0674947, 0, 0.0854409, 0, 0.0577979, 0, 0.0659504, 0, 0.0653065, 0, 0.0765228, 0, 0.0806947, 0, 0.0826921, 0, 0.111367, 0, 0.145509, 0, 0.171511, 0, 0.189475, 0, 0.204345, 0, 0.213035, 0, 0.227076, 0, 0.228418, 0, 0.225342, 0.226159, 0.238377, 0.258902, 0.272277, 0.296012, 0.310416, 0.324791, 0.344087, 0.36105, 0.375296, 0.390572, 0.396333, 0.402745, 0.408727, 0.408912, 0.40714, 0.402191, 0.409314, 0.419363, 0.431893, 0.449467, 0.476084, 0.495188, 0.527823, 0.549294, 0.573612, 0.596173, 0.613573, 0.624919, 0.637112, 0.643261, 0.646571, 0.639317, 0.631578, 0.624705, 0.618116, 0.619289, 0.61712, 0.618667, 0.617283, 0.617378, 0.620017, 0.6131, 0.607222, 0.598935, 0.589784, 0.58948, 0.599273, 0.607069, 0.636253, 0.656746, 0.689944, 0.71838, 0.740457, 0.754486, 0.76724, 0.764127, 0.769767, 0.766321, 0.75666, 0.749375, 0.751232, 0.757399, 0.766103, 0.773496, 0.774275, 0.783016, 0.79393, 0.785091, 0.784351, 0.776532, 0.760985, 0.745423, 0.732718, 0.715433, 0.70845, 0.692548, 0.674702, 0.654253, 0.634408, 0.622252, 0.608187, 0.592773, 0.577007, 0.564583, 0.552195, 0.537669, 0.530178, 0.531312, 0.535306, 0.539937, 0.554178, 0.561923, 0.576778, 0.575171, 0.583637, 0.584305, 0.583309, 0.570455, 0.571451, 0.569682, 0.570178, 0.567912, 0.564456, 0.552357, 0.547321, 0.554199, 0.554802, 0.557577, 0.541557, 0.529159, 0.524846, 0.514135, 0.491894, 0.488949, 0.479805, 0.473978, 0.471848, 0.456823, 0.441123, 0.41791, 0.382769, 0.341407, 0.309601, 0.266613, 0.234818, 0.211879, 0.198919, 0.191333, 0.19997, 0.197148, 0.195367, 0.207265, 0.212239, 0.207493, 0.194859, 0.182277, 0.17847, 0.182714, 0.178229, 0.195465, 0.220442, 0.257119, 0.285303, 0.30773, 0.320499, 0.328481, 0.319334, 0.298033, 0.272132, 0.244204, 0.220292, 0.201763, 0.190125, 0.188096, 0.202331, 0.223352, 0.243833, 0.273417, 0.304988, 0.33048, 0.349205, 0.358154, 0.361181, 0.367281, 0.372139, 0.385897, 0.407124, 0.430055, 0.458109, 0.481058, 0.502945, 0.514345, 0.51672, 0.507268, 0.495423, 0.480187, 0.469933, 0.454461, 0.441309, 0.43554, 0.437399, 0.452636, 0.466134, 0.478342, 0.496863, 0.523671, 0.543875, 0.552916, 0.55532, 0.561626, 0.572234, 0.587939, 0.603518, 0.618622, 0.650915, 0.672798, 0.697117, 0.709121, 0.713141, 0.714528, 0.718779, 0.711137, 0.715272, 0.715036, 0.722707, 0.742943, 0.751166, 0.766531, 0.783849, 0.792786, 0.796254, 0.800882, 0.793684, 0.796304, 0.788639, 0.781428, 0.773167, 0.770007, 0.76426, 0.768416, 0.772481, 0.777702, 0.781475, 0.77507, 0.768292, 0.768393, 0.749513, 0.720583, 0.700534, 0.67835, 0.666653, 0.656801, 0.627893, 0.611559, 0.607249, 0.606637, 0.609656, 0.612958, 0.608817, 0.623134, 0.638703, 0.650259, 0.651039, 0.651369, 0.649076, 0.661765, 0.668292, 0.677887, 0.682207, 0.672743, 0.663898, 0.655535, 0.632248, 0.611078, 0.587861, 0.568376, 0.554236, 0.529973, 0.503604, 0.476598, 0.449229, 0.42971, 0.415706, 0.412413, 0.411465, 0.414582, 0.425301, 0.427433, 0.42041, 0.403611, 0.383181, 0.369193, 0.360098, 0.362434, 0.375445, 0.378094, 0.385489, 0.380722, 0.379705, 0.374025, 0.34755, 0.319084, 0.294557, 0.275827, 0.264671, 0.249873, 0.23906, 0.235499, 0.226895, 0.221713, 0.209259, 0.207938, 0.213753, 0.218633, 0.224483, 0.227365, 0.237877, 0.247965, 0.254027, 0.265903, 0.283401, 0.297746, 0.319091, 0.32932, 0.347067, 0.354841, 0.353944, 0.339155, 0.320471, 0.312564, 0.308612, 0.307105, 0.315013, 0.31711, 0.320471, 0.327823, 0.315977, 0.303035, 0.298695, 0.302188, 0.307595, 0.322737, 0.346558, 0.383879, 0.431151, 0.456426, 0.472199, 0.483127, 0.48508, 0.486523, 0.491945, 0.485239, 0.494064, 0.49491, 0.496438, 0.509835, 0.527472, 0.539186, 0.552908, 0.555047, 0.56038, 0.571938, 0.574953, 0.558559, 0.545108, 0.539583, 0.535527, 0.538416, 0.542642, 0.55028, 0.575437, 0.590169, 0.596982, 0.601882, 0.598338, 0.591148, 0.577367, 0.555103, 0.543165, 0.529925, 0.522546, 0.524165, 0.534606, 0.550365, 0.571828, 0.592844, 0.615078, };

			double[] KalmanVel = new double[] { -0.0000788, 0.6629258, -0.3105288, 0.8913171, 1.1070651, 0.0973066, -0.1061661, -0.7096730, -0.5200728, -0.5036552, -0.1990076, -0.0037548, -0.0516418, 0.1353790, 0.1088101, 0.0926194, 0.0950710, 0.1986139, 0.1229017, 0.0795166, 0.1022972, 0.1945099, 0.2074429, 0.2444296, 0.3141527, 0.2594841, 0.2755021, 0.3120331, 0.3854845, 0.3798421, 0.3745762, 0.4089395, 0.4104751, 0.3917160, 0.4056575, 0.4210419, 0.4278872, 0.4328291, 0.4550818, 0.4670931, 0.4760410, 0.4841370, 0.4943129, 0.5075290, 0.5134419, 0.5286533, 0.5442726, 0.5570630, 0.5650558, 0.5772091, 0.5712518, 0.5807934, 0.5924332, 0.5978596, 0.6026945, 0.6208083, 0.6312256, 0.6449437, 0.6569851, 0.6663537, 0.6786649, 0.6657235, 0.6839709, 0.6812787, 0.6869317, 0.6920671, 0.6980456, 0.7153946, 0.7331517, 0.7325536, 0.7466403, 0.7580417, 0.7639138, 0.7636899, 0.7576445, 0.7631599, 0.7626229, 0.7624766, 0.7585275, 0.7546889, 0.7559299, 0.7488587, 0.7400421, 0.7439018, 0.7551811, 0.7597204, 0.7684077, 0.7635000, 0.7726856, 0.7647306, 0.7717353, 0.7642419, 0.7539558, 0.7486908, 0.7386482, 0.7331047, 0.7218524, 0.7098390, 0.7010186, 0.7070475, 0.7036152, 0.6983864, 0.6958418, 0.6789065, 0.6714247, 0.6629908, 0.6570735, 0.6458188, 0.6370295, 0.6305704, 0.6141444, 0.5970489, 0.5814649, 0.5706124, 0.5512478, 0.5468515, 0.5449511, 0.5401673, 0.5275620, 0.5052481, 0.4834573, 0.4818923, 0.4733375, 0.4717328, 0.4669977, 0.4561801, 0.4472860, 0.4381833, 0.4341320, 0.4330173, 0.4340540, 0.4274889, 0.4173550, 0.4043941, 0.3806869, 0.3673888, 0.3643606, 0.3459575, 0.3379461, 0.3300108, 0.3268827, 0.3131659, 0.3155235, 0.3086024, 0.3037932, 0.2951102, 0.2858447, 0.2824171, 0.2723280, 0.2637725, 0.2559921, 0.2567699, 0.2465068, 0.2343922, 0.2293652, 0.2254336, 0.2289439, 0.2252859, 0.2244515, 0.2227348, 0.2237876, 0.2305019, 0.2235628, 0.2117941, 0.2098808, 0.2056473, 0.2027213, 0.2053242, 0.2063029, 0.1984643, 0.2015260, 0.2092136, 0.2100579, 0.2056681, 0.2136808, 0.2144865, 0.2209954, 0.2295927, 0.2391043, 0.2326427, 0.2440291, 0.2453421, 0.2583347, 0.2731693, 0.2881365, 0.2917728, 0.2975728, 0.3028847, 0.3163120, 0.3314596, 0.3383836, 0.3466609, 0.3538412, 0.3645217, 0.3744558, 0.3841028, 0.3957277, 0.4110478, 0.4214815, 0.4266813, 0.4375950, 0.4445745, 0.4503813, 0.4609163, 0.4724961, 0.4895663, 0.5027529, 0.5239466, 0.5347674, 0.5426332, 0.5561034, 0.5606951, 0.5748590, 0.5843227, 0.5878049, 0.5941907, 0.6061183, 0.6014067, 0.6162437, 0.6278360, 0.6362910, 0.6444928, 0.6465838, 0.6503481, 0.6611547, 0.6641712, 0.6714695, 0.6678033, 0.6716784, 0.6636246, 0.6626305, 0.6653779, 0.6738135, 0.6801584, 0.6830258, 0.6878164, 0.6870592, 0.6822284, 0.6835896, 0.6905691, 0.6954334, 0.6916214, 0.6839646, 0.6809171, 0.6806750, 0.6786540, 0.6690439, 0.6589345, 0.6589230, 0.6523689, 0.6461502, 0.6389231, 0.6395538, 0.6320413, 0.6116976, 0.6027445, 0.5976180, 0.6022745, 0.5915009, 0.5713828, 0.5557959, 0.5460942, 0.5475354, 0.5433959, 0.5412684, 0.5333885, 0.5295199, 0.5147578, 0.5150818, 0.5099758, 0.5047768, 0.4926168, 0.4892125, 0.4828215, 0.4619226, 0.4566431, 0.4617303, 0.4450580, 0.4331711, 0.4226909, 0.4057998, 0.3953208, 0.3886621, 0.3737860, 0.3674242, 0.3597262, 0.3526033, 0.3374659, 0.3242128, 0.3166142, 0.3038483, 0.2775999, 0.2843416, 0.2881136, 0.2837488, 0.2868990, 0.2760283, 0.2644102, 0.2614093, 0.2586488, 0.2492207, 0.2431762, 0.2418320, 0.2474188, 0.2407069, 0.2398129, 0.2333341, 0.2330788, 0.2301763, 0.2323012, 0.2353719, 0.2326539, 0.2203227, 0.2284171, 0.2292188, 0.2419292, 0.2434854, 0.2468378, 0.2428507, 0.2467496, 0.2628121, 0.2578710, 0.2618778, 0.2673093, 0.2796944, 0.2849528, 0.2881330, 0.2873697, 0.2910699, 0.2930593, 0.2962942, 0.2970467, 0.2997326, 0.3100196, 0.3188120, 0.3324062, 0.3390010, 0.3418593, 0.3491901, 0.3570362, 0.3623362, 0.3753314, 0.3893890, 0.3948940, 0.4129186, 0.4271812, 0.4240580, 0.4265073, 0.4213474, 0.4283142, 0.4488232, 0.4598646, 0.4679537, 0.4745997, 0.4885448, 0.4965083, 0.5078021, 0.5065548, 0.5098977, 0.5233306, 0.5357927, 0.5508283, 0.5697913, 0.5909590, 0.5967873, 0.6122737, 0.6267025, 0.6352773, 0.6376782, 0.6411758, 0.6360042, 0.6372523, 0.6476722, 0.6518291, 0.6685214, 0.6735477, 0.6755573, 0.6833268, 0.6924098, 0.7014948, 0.7076913, 0.7137627, 0.7243994, 0.7333354, 0.7464834, 0.7474673, 0.7573891, 0.7601911, 0.7656888, 0.7656049, 0.7650830, 0.7616771, 0.7599331, 0.7643192, 0.7630991, 0.7591767, 0.7571161, 0.7602545, 0.7635405, 0.7558103, };

			//const double std_a = 0.0069;
			const double std_a = 0.6;
			//const double std_h =1;
			const double std_h = 0.1298;

			const int n = 200;
			const double dT = 0.04;
			double startVelocity = 0.5;
			
			double v = 0;
			double h = 0;
			double a = 0;
			double sumDifference = 0;
			Random rand = new Random();
			double lastVelocity = -0.25 * Math.Cos(0) + startVelocity;
			Console.WriteLine(lastVelocity);
			List<Tuple<float, float, float, float, float, float, float, Tuple<float>>> allValues = new List<Tuple<float, float, float, float, float, float, float, Tuple<float>>>(); //Zeit //SimHei //SimVel //SimAcce //measHei //measAcce //velLinearRegression //KalmanVel
			for (int i = 0; i < n; i++)
			{
				double time = dT * i;
				a = 0.25 * Math.Sin(time);
				v = -0.25 * Math.Cos(time) + startVelocity;
				h = -0.25 * Math.Sin(time) + startVelocity*time;
				if(time > 2)
					sumDifference += (v - KalmanVel[i]) * (v - KalmanVel[i]);
				//double x = ((double)i / (double)n) * 15.4;
				//a = -0.00000598030247192407 * Math.Pow(x, 6) + 0.000335989279260762 * Math.Pow(x, 5) - 0.00731672013991802 * Math.Pow(x, 4) + 0.076243032770055 * Math.Pow(x, 3) - 0.37462261210635700000 * Math.Pow(x, 2) + 0.711052676821694 * x;
				
				//simAcc[i] = a;
				//simVel[i] = v;
				//h += v * dT + (a * dT * dT)/2;
				//v += a * dT;
				//simHei[i] = h;
				double measAcceleration = randomGauss(rand ,a, std_a) + 0.05;
				lastVelocity = lastVelocity + measAcceleration * dT;
				double measHeight = lastVelocity;
				//double measAcceleration = a;
				//double measHeight = randomGauss(rand, h, std_h);
				//allValues.Add(new Tuple<float, float, float, float, float, float, float, Tuple<float>>((float)i * (float)dT, (float)h, (float)v, (float)a, (float)measHeight, (float)measAcceleration, 0, new Tuple<float>(0)));
				allValues.Add(new Tuple<float, float, float, float, float, float, float, Tuple<float>>((float)i* (float)dT, (float)h, (float)v, (float)a, (float)measHeight, (float)measAcceleration, (float)linearRegressionVel[i], new Tuple<float>((float)KalmanVel[i])));
			}
			Console.WriteLine(Math.Sqrt(sumDifference / n));
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

		public static void WriteToExcel(List<Tuple<float, float, float, float, float, float, float, Tuple<float>>> values)
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
				excelWorksheet.Cells[1, 7] = "VelLinearRegression";
				excelWorksheet.Cells[1, 8] = "VelKalman";
				for (int y = 0; y < values.Count; y++)
				{
					excelWorksheet.Cells[y + 3, 1] = values[y].Item1;
					excelWorksheet.Cells[y + 3, 2] = values[y].Item2;
					excelWorksheet.Cells[y + 3, 3] = values[y].Item3;
					excelWorksheet.Cells[y + 3, 4] = values[y].Item4;
					excelWorksheet.Cells[y + 3, 5] = values[y].Item5;
					excelWorksheet.Cells[y + 3, 6] = values[y].Item6;
					excelWorksheet.Cells[y + 3, 7] = values[y].Item7;
					excelWorksheet.Cells[y + 3, 8] = values[y].Rest.Item1;
				}

				/*Excel.Range chartRange;

				Excel.ChartObjects xlCharts = (Excel.ChartObjects)excelWorksheet.ChartObjects(Type.Missing);
				Excel.ChartObject myChart = (Excel.ChartObject)xlCharts.Add(10, 80, 300, 250);
				Excel.Chart chartPage = myChart.Chart;

				chartRange = excelWorksheet.Application.Union(excelWorksheet.get_Range("A3", "A302"), excelWorksheet.get_Range("H3", "H302"));
				chartPage.SetSourceData(chartRange);
				chartPage.ChartType = Excel.XlChartType.xlLine;*/

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
