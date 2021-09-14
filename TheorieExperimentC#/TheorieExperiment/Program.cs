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
			double[] linearRegressionVel = new double[] { 0, 0, 0, 0, 0, 0.0000000, 0, 2.6629384, 0, 3.0378902, 0, 1.0891211, 0, 1.7608947, 0, 2.1088326, 0, 1.6738454, 0, 1.3656135, 0, 0.8674574, 0, 0.4076905, 0, 0.0455853, 0, -0.1960931, 0, -0.5797653, 0, -0.6741362, 0, -0.7689212, 0, -0.7032633, 0, -0.6073616, 0, -0.3963120, 0, -0.2635538, 0, -0.1126923, 0, -0.0223760, 0, -0.0225912, 0, -0.0664971, 0, -0.0757551, 0, -0.0829513, 0, -0.1095344, 0, -0.1365213, 0, -0.1437130, 0, -0.1239453, 0, -0.0881072, 0, -0.0436205, 0, 0.0309364, 0, 0.0892114, 0, 0.1250046, 0, 0.1370069, 0, 0.1555418, 0, 0.1672907, 0, 0.2015344, 0, 0.2119032, 0, 0.1976143, 0, 0.1770015, 0, 0.1576372, 0, 0.1106030, 0, 0.0646965, 0, 0.0317262, 0, -0.0061247, 0, -0.0504736, 0, -0.0659941, 0, -0.0741331, 0, -0.0642943, 0, -0.0552799, 0, -0.0372572, 0, -0.0356280, 0, -0.0226257, 0, -0.0331308, 0, -0.0358157, 0, -0.0445854, 0, -0.0413824, 0, -0.0465809, 0, -0.0296721, 0, -0.0086697, 0, 0.0064751, 0, 0.0228140, 0, 0.0424860, 0, 0.0606383, 0, 0.0734551, 0, 0.0897614, 0, 0.0969402, 0, 0.1044482, 0, 0.0965113, 0, 0.0949664, 0, 0.0986930, 0, 0.1072554, 0, 0.1105256, 0, 0.1200087, 0, 0.1240664, 0, 0.1246476, 0, 0.1261885, 0, 0.1296333, 0, 0.1327252, 0, 0.1357791, 0, 0.1382837, 0, 0.1345753, 0, 0.1352986, 0, 0.1374859, 0, 0.1384994, 0, 0.1393214, 0, 0.1404887, 0, 0.1392002, 0, 0.1384814, 0.1387543, 0.1420293, 0.1502796, 0.1632804, 0.1720382, 0.1823738, 0.1939531, 0.2048189, 0.2157818, 0.2282016, 0.2466780, 0.2610372, 0.2756428, 0.2876091, 0.2980478, 0.3028401, 0.3123509, 0.3186939, 0.3267242, 0.3365628, 0.3513927, 0.3630785, 0.3731057, 0.3779546, 0.3854009, 0.3932772, 0.3984083, 0.3997694, 0.3973901, 0.3957588, 0.3922707, 0.3915407, 0.3948165, 0.4023931, 0.4059379, 0.4072950, 0.4098221, 0.4183995, 0.4314637, 0.4422494, 0.4425826, 0.4418842, 0.4348104, 0.4169188, 0.3970274, 0.3865956, 0.3680924, 0.3477957, 0.3380529, 0.3278805, 0.3226987, 0.3221042, 0.3230694, 0.3204381, 0.3228015, 0.3144674, 0.3121533, 0.3051025, 0.2991787, 0.2897424, 0.2883272, 0.2822763, 0.2754715, 0.2691883, 0.2658596, 0.2622814, 0.2592510, 0.2596879, 0.2522180, 0.2437295, 0.2253366, 0.2094094, 0.1991536, 0.1947965, 0.1895550, 0.1907106, 0.1893446, 0.1820249, 0.1733723, 0.1673132, 0.1588813, 0.1511870, 0.1451996, 0.1343579, 0.1240607, 0.1140929, 0.1020394, 0.0872458, 0.0806676, 0.0671989, 0.0559968, 0.0428818, 0.0367776, 0.0292272, 0.0309920, 0.0324047, 0.0368696, 0.0374260, 0.0413983, 0.0423288, 0.0449692, 0.0550335, 0.0613291, 0.0674228, 0.0789159, 0.0834270, 0.0851484, 0.0905659, 0.0899644, 0.0865703, 0.0894881, 0.0929512, 0.0911123, 0.0960899, 0.0987478, 0.1044088, 0.1195554, 0.1413115, 0.1576562, 0.1701669, 0.1851194, 0.1858288, 0.1868159, 0.1890914, 0.1940850, 0.1929101, 0.1952664, 0.1989927, 0.2132119, 0.2246262, 0.2344320, 0.2361880, 0.2373036, 0.2290471, 0.2201810, 0.2137613, 0.2218802, 0.2197965, 0.2145589, 0.2172569, 0.2216715, 0.2251956, 0.2355495, 0.2449566, 0.2533604, 0.2599569, 0.2622909, 0.2717924, 0.2802341, 0.2844563, 0.2920677, 0.2977321, 0.2940185, 0.2976814, 0.3057896, 0.3141142, 0.3191839, 0.3301142, 0.3383953, 0.3395315, 0.3400417, 0.3412307, 0.3429984, 0.3468412, 0.3546475, 0.3632031, 0.3736696, 0.3843056, 0.3936349, 0.3952233, 0.3970912, 0.3993815, 0.4039568, 0.4065851, 0.4148410, 0.4143123, 0.4086722, 0.3939909, 0.3775097, 0.3716459, 0.3672853, 0.3676530, 0.3710494, 0.3830110, 0.3859399, 0.3920945, 0.3966992, 0.4057264, 0.4080520, 0.4135237, 0.4164184, 0.4164031, 0.4157905, 0.4159051, 0.4146998, 0.4175255, 0.4186453, 0.4218660, 0.4255033, 0.4197451, 0.4149890, 0.4104919, 0.4008098, 0.3843673, 0.3757999, };

			double[] KalmanVel = new double[] { -0.0000440, -0.0036230, -0.0051446, -0.0071375, -0.0057980, 0.0003162, 0.0043861, 0.0062988, 0.0104964, 0.0174536, 0.0222831, 0.0303444, 0.0410118, 0.0497534, 0.0571588, 0.0624098, 0.0717671, 0.0833713, 0.0926126, 0.1015428, 0.1125966, 0.1239463, 0.1344653, 0.1446937, 0.1542843, 0.1665806, 0.1826618, 0.1989819, 0.2177157, 0.2370109, 0.2501006, 0.2591392, 0.2726136, 0.2851573, 0.2982902, 0.3123174, 0.3272550, 0.3429872, 0.3554819, 0.3665823, 0.3834223, 0.4013514, 0.4187582, 0.4301328, 0.4494243, 0.4630987, 0.4733084, 0.4925103, 0.5031570, 0.5225142, 0.5393479, 0.5545905, 0.5672086, 0.5832587, 0.6067004, 0.6182833, 0.6336398, 0.6537243, 0.6658003, 0.6744462, 0.6805412, 0.6925562, 0.7002322, 0.7052574, 0.7207053, 0.7334337, 0.7432420, 0.7434511, 0.7550567, 0.7641336, 0.7690374, 0.7807835, 0.7841424, 0.7769266, 0.7801926, 0.7796815, 0.7699011, 0.7639233, 0.7623802, 0.7542025, 0.7502182, 0.7485504, 0.7536046, 0.7515201, 0.7552632, 0.7440822, 0.7346827, 0.7264295, 0.7101567, 0.6961760, 0.6931798, 0.6796194, 0.6698728, 0.6583831, 0.6502547, 0.6348241, 0.6331801, 0.6121872, 0.6057866, 0.5948487, 0.5863665, 0.5779735, 0.5715265, 0.5624784, 0.5560105, 0.5395930, 0.5295930, 0.5102873, 0.4891571, 0.4679901, 0.4449322, 0.4340005, 0.4153726, 0.4123351, 0.4083878, 0.4010039, 0.3870184, 0.3781704, 0.3662457, 0.3516581, 0.3321592, 0.3197881, 0.3026213, 0.2900927, 0.2807644, 0.2574052, 0.2332734, 0.2114927, 0.1924607, 0.1706179, 0.1647475, 0.1541350, 0.1327941, 0.1149520, 0.1131161, 0.1008132, 0.0846970, 0.0720322, 0.0636383, 0.0500147, 0.0432903, 0.0294513, 0.0275787, 0.0184019, 0.0156092, 0.0056390, 0.0004527, -0.0022170, -0.0070357, -0.0015736, -0.0068147, -0.0006938, -0.0010028, 0.0016187, 0.0116152, 0.0169058, 0.0239915, 0.0323954, 0.0425036, 0.0493609, 0.0623854, 0.0574641, 0.0598135, 0.0672746, 0.0850780, 0.0902193, 0.0989807, 0.1042769, 0.1208817, 0.1368879, 0.1426422, 0.1551575, 0.1679219, 0.1893305, 0.2001986, 0.2177680, 0.2298942, 0.2377352, 0.2543517, 0.2663384, 0.2738199, 0.2955604, 0.3064561, 0.3143224, 0.3203115, 0.3259435, 0.3291677, 0.3392969, 0.3484434, 0.3557014, 0.3760315, 0.3969420, 0.4020974, 0.4135734, 0.4316493, 0.4405538, 0.4626653, 0.4720617, 0.4692022, 0.4754839, 0.5031408, 0.5131288, 0.5165614, 0.5360575, 0.5477585, 0.5600004, 0.5742458, 0.5840841, 0.5900317, 0.6072327, 0.6224960, 0.6298368, 0.6225414, 0.6342527, 0.6426661, 0.6504756, 0.6641554, 0.6700819, 0.6748496, 0.6815357, 0.6778815, 0.6778698, 0.6867697, 0.7020313, 0.6953638, 0.6940909, 0.6959150, 0.6909156, 0.6758190, 0.6752768, 0.6759132, 0.6710150, 0.6719289, 0.6580269, 0.6493074, 0.6548074, 0.6524756, 0.6459498, 0.6218443, 0.6146344, 0.6150452, 0.6182902, 0.6078660, 0.6090510, 0.6004140, 0.5954711, 0.5835922, 0.5719590, 0.5697010, 0.5554321, 0.5279361, 0.5126594, 0.4917364, 0.4781945, 0.4586957, 0.4335924, 0.4179226, 0.4077547, 0.3997300, 0.3903954, 0.3763489, 0.3569655, 0.3417512, 0.3224233, 0.3157753, 0.3035387, 0.3008267, 0.2889700, 0.2745855, 0.2548676, 0.2281307, 0.2196845, 0.2068018, 0.1943388, 0.1795674, 0.1753911, 0.1706004, 0.1604436, 0.1436257, 0.1267676, 0.1050809, 0.0958077, 0.0775720, 0.0717589, 0.0519640, 0.0417207, 0.0319493, 0.0385253, 0.0178079, 0.0065101, -0.0018793, -0.0152291, -0.0238818, -0.0278358, -0.0355695, -0.0340886, -0.0295789, -0.0381786, -0.0379562, -0.0308544, };

			const double std_a = 0.075;
			const double std_h = 0.5;
			const int n = 300;
			const double dT = 0.03;
			//double[] simAcc = new double[n];
			//double[] simVel = new double[n];
			//double[] simHei = new double[n];
			//double[] measAcc = new double[n];
			//double[] measHei = new double[n];
			double v = 0;
			double h = 0;
			double a = 0;
			Random rand = new Random();
			List<Tuple<float, float, float, float, float, float, float, Tuple<float>>> allValues = new List<Tuple<float, float, float, float, float, float, float, Tuple<float>>>(); //Zeit //SimHei //SimVel //SimAcce //measHei //measAcce //velLinearRegression //KalmanVel
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
				allValues.Add(new Tuple<float, float, float, float, float, float, float, Tuple<float>>((float)i* (float)dT, (float)h, (float)v, (float)a, (float)measHeight, (float)measAcceleration, (float)linearRegressionVel[i], new Tuple<float>((float)KalmanVel[i])));
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
