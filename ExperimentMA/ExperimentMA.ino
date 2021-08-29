#include "LinearRegression.h"
#include "Variometer.h"
#include "FiFo.h"

#define lengthLinearRegression 16

#include <Kalman.h>
using namespace BLA;

#define Nstate 3 // position, speed, acceleration
#define Nobs 2   // position, acceleration

// measurement std
#define n_p 0.3
#define n_a 5.0
// model std (1/inertia)
#define m_p 0.1
#define m_s 0.1
#define m_a 0.8

double times[300] = { 0, 0.05, 0.1, 0.15, 0.2, 0.25, 0.3, 0.35, 0.4, 0.45, 0.5, 0.55, 0.6, 0.65, 0.7, 0.75, 0.8, 0.85, 0.9, 0.95, 1, 1.05, 1.1, 1.15, 1.2, 1.25, 1.3, 1.35, 1.4, 1.45, 1.5, 1.55, 1.6, 1.65, 1.7, 1.75, 1.8, 1.85, 1.9, 1.95, 2, 2.05, 2.1, 2.15, 2.2, 2.25, 2.3, 2.35, 2.4, 2.45, 2.5, 2.55, 2.6, 2.65, 2.7, 2.75, 2.8, 2.85, 2.9, 2.95, 3, 3.05, 3.1, 3.15, 3.2, 3.25, 3.3, 3.35, 3.4, 3.45, 3.5, 3.55, 3.6, 3.65, 3.7, 3.75, 3.8, 3.85, 3.9, 3.95, 4, 4.05, 4.1, 4.15, 4.2, 4.25, 4.3, 4.35, 4.4, 4.45, 4.5, 4.55, 4.6, 4.65, 4.7, 4.75, 4.8, 4.85, 4.9, 4.95, 5, 5.05, 5.1, 5.15, 5.2, 5.25, 5.3, 5.35, 5.4, 5.45, 5.5, 5.55, 5.6, 5.65, 5.7, 5.75, 5.8, 5.85, 5.9, 5.95, 6, 6.05, 6.1, 6.15, 6.2, 6.25, 6.3, 6.35, 6.4, 6.45, 6.5, 6.55, 6.6, 6.65, 6.7, 6.75, 6.8, 6.85, 6.9, 6.95, 7, 7.05, 7.1, 7.15, 7.2, 7.25, 7.3, 7.35, 7.4, 7.45, 7.5, 7.55, 7.6, 7.65, 7.7, 7.75, 7.8, 7.85, 7.9, 7.95, 8, 8.05, 8.1, 8.150001, 8.2, 8.25, 8.3, 8.35, 8.400001, 8.45, 8.5, 8.55, 8.6, 8.650001, 8.7, 8.75, 8.8, 8.85, 8.900001, 8.95, 9, 9.05, 9.1, 9.150001, 9.2, 9.25, 9.3, 9.35, 9.400001, 9.45, 9.5, 9.55, 9.6, 9.650001, 9.7, 9.75, 9.8, 9.85, 9.900001, 9.95, 10, 10.05, 10.1, 10.15, 10.2, 10.25, 10.3, 10.35, 10.4, 10.45, 10.5, 10.55, 10.6, 10.65, 10.7, 10.75, 10.8, 10.85, 10.9, 10.95, 11, 11.05, 11.1, 11.15, 11.2, 11.25, 11.3, 11.35, 11.4, 11.45, 11.5, 11.55, 11.6, 11.65, 11.7, 11.75, 11.8, 11.85, 11.9, 11.95, 12, 12.05, 12.1, 12.15, 12.2, 12.25, 12.3, 12.35, 12.4, 12.45, 12.5, 12.55, 12.6, 12.65, 12.7, 12.75, 12.8, 12.85, 12.9, 12.95, 13, 13.05, 13.1, 13.15, 13.2, 13.25, 13.3, 13.35, 13.4, 13.45, 13.5, 13.55, 13.6, 13.65, 13.7, 13.75, 13.8, 13.85, 13.9, 13.95, 14, 14.05, 14.1, 14.15, 14.2, 14.25, 14.3, 14.35, 14.4, 14.45, 14.5, 14.55, 14.6, 14.65, 14.7, 14.75, 14.8, 14.85, 14.9, 14.95,  };
double measHeights[300] = { 0.01181455, 0.4271037, -0.2105181, 0.1635813, 0.553017, 0.6644368, 0.7555178, 0.3284283, 0.06225647, -0.3409612, 0.5797161, 0.5234606, -0.5155812, -0.5106127, -0.04642811, -0.1929161, 0.3924072, 0.6995417, 0.5222009, 0.2463908, 0.1672519, 0.201016, -0.3584776, -0.3557895, -0.09803481, -0.6019927, 0.2747729, 0.1129105, 0.4827724, 0.651889, -0.4549325, -1.060251, 0.02281244, 0.1267837, 0.2435463, 0.2354598, 0.1876565, -0.04937547, -0.2453619, -0.1715844, 0.2221244, -0.04724286, 0.6773889, 0.1685883, 1.260402, 0.3630558, -0.1002144, 1.429951, 0.1573648, 1.083356, 1.032165, 0.9222907, 0.390759, 0.624643, 1.634149, 0.8154474, 1.218384, 1.660328, 1.1158, 0.9821513, 0.7797624, 0.9965572, 0.4689424, 0.5072809, 1.517231, 1.397467, 1.406533, 0.5885385, 1.517743, 1.685362, 1.641369, 2.02418, 1.371728, 0.6652447, 1.608421, 1.3969, 0.6709818, 1.002725, 1.340218, 0.8484814, 1.199803, 1.335621, 2.033545, 1.799376, 2.553925, 1.526856, 1.495262, 1.376372, 0.9592288, 1.410946, 2.251968, 1.432392, 1.726215, 1.765218, 1.84283, 1.45962, 3.064227, 1.706792, 2.790887, 2.445501, 2.673694, 2.708576, 2.767984, 2.487938, 2.72045, 2.016042, 2.33683, 1.827516, 1.875673, 1.829567, 1.747356, 2.822761, 2.209984, 3.296289, 3.239407, 3.238432, 2.664241, 2.924201, 2.878572, 2.701926, 2.358981, 2.950296, 2.571811, 3.023776, 3.053682, 1.907612, 1.885628, 2.058121, 2.296533, 2.286761, 3.58748, 3.122004, 2.419782, 2.601441, 3.729644, 2.695992, 2.246606, 2.366271, 2.732143, 2.236002, 2.614163, 2.041304, 3.129997, 2.543405, 2.813785, 2.407693, 2.84912, 2.717713, 2.328649, 3.000228, 2.162457, 3.162082, 2.661111, 3.003748, 3.730547, 3.172786, 2.971944, 3.206008, 3.30523, 2.894735, 3.311081, 2.028609, 2.509257, 2.620724, 3.305977, 2.320585, 2.878729, 2.7393, 3.579584, 3.442232, 2.331319, 2.837035, 3.042047, 3.727867, 2.992156, 3.361049, 2.719457, 2.810199, 3.540808, 3.049448, 2.666861, 3.738018, 2.909451, 2.693987, 2.540734, 2.235155, 1.965737, 2.813434, 2.637834, 2.233541, 3.319916, 3.629135, 2.432545, 2.875449, 3.656479, 2.879932, 3.668143, 2.743617, 2.096349, 2.698985, 4.345314, 3.138583, 2.700725, 3.980577, 3.368798, 3.390448, 3.527558, 3.438806, 3.320437, 4.40153, 4.351915, 3.821362, 2.641474, 4.145102, 3.781757, 3.741776, 4.473626, 3.919516, 3.979373, 4.234186, 3.400349, 3.662799, 4.534249, 5.205804, 3.336801, 3.888476, 4.173614, 3.87253, 3.046297, 4.056013, 4.22893, 4.175008, 4.888234, 3.630354, 4.077841, 5.28293, 4.628601, 4.355717, 2.980685, 4.453949, 4.872128, 5.304388, 4.514143, 5.33436, 4.745757, 5.393072, 4.966506, 4.774102, 5.297811, 4.633213, 3.746816, 4.69344, 4.21667, 5.089763, 4.686929, 4.196998, 4.884471, 5.205314, 5.424504, 5.425697, 5.252738, 4.781552, 4.990206, 4.495396, 5.419832, 5.063952, 6.069798, 5.547427, 5.36332, 4.933406, 4.627554, 6.013276, 5.560995, 5.671389, 5.348448, 5.835206, 5.789145, 5.446028, 5.1006, 5.075351, 4.553168, 5.371661, 4.507927, 5.815586, 4.717052, 5.241574, 5.102326, 6.17342, 4.256921, 5.171512, 5.160343, 4.51837, 5.156661, 5.569389, 4.950601, 5.517729, 5.678913, 4.743628, 5.477175, 5.917281,  };
double measAccelerations[300] = { -0.1375563, -0.003611138, -0.03605135, -0.05543105, 0.2093548, 0.08609071, 0.02841667, 0.007903215, 0.2375362, 0.06018415, 0.08902579, 0.3148365, 0.1444652, 0.1987487, 0.04573652, 0.159523, 0.2770084, 0.1658877, 0.1374088, 0.2228433, 0.2518087, 0.1823949, 0.2924768, 0.1161451, 0.2955367, 0.332594, 0.3121138, 0.3566149, 0.3690817, 0.32354, 0.2235101, 0.301878, 0.2557454, 0.2249054, 0.3040472, 0.2551343, 0.3883341, 0.3168229, 0.2394997, 0.2805522, 0.5006229, 0.309824, 0.190902, 0.2800455, 0.2447615, 0.3351105, 0.1989971, 0.229951, 0.3317823, 0.2582373, 0.2014482, 0.2882817, 0.3262424, 0.3375302, 0.179436, 0.1698385, 0.2737362, 0.1688415, 0.1521684, 0.1021738, 0.2003296, 0.3344326, 0.1775543, 0.1840939, 0.208299, 0.1157391, 0.09178475, 0.184716, 0.1252637, -0.08131744, 0.0788524, 0.1077926, 0.004826761, 0.03565872, -0.008040258, 0.01458942, -0.00940057, 0.02755423, 0.0185219, 0.001346792, 0.03996642, 0.04913583, -0.08273243, -0.1156659, -0.1970114, -0.1285924, -0.02436775, -0.02297976, -0.2614366, -0.08595456, -0.2147152, -0.04280591, -0.2621804, -0.08639872, -0.1154169, -0.2825492, -0.3879366, -0.2584952, -0.3000615, -0.2493252, -0.3183941, -0.2082286, -0.2502073, -0.133527, -0.315969, -0.1225979, -0.1975653, -0.3393674, -0.2416005, -0.2971714, -0.2895876, -0.3399379, -0.2473551, -0.1976513, -0.2979843, -0.358734, -0.1632201, -0.3032997, -0.2972091, -0.2809949, -0.3417545, -0.2338575, -0.3920928, -0.2086477, -0.24625, -0.2632397, -0.2860651, -0.2345484, -0.3267755, -0.3789559, -0.2193765, -0.3739646, -0.3247193, -0.2665297, -0.1963384, -0.1957335, -0.1366444, -0.1251734, -0.1918985, -0.04704272, -0.1166659, -0.08469258, -0.2081671, -0.01897899, -0.07586881, -0.2131237, -0.01272475, -0.007157998, 0.07243066, 0.05410603, -0.003084292, 0.0368668, -0.02415825, -0.01254786, -0.08746248, 0.2037731, 0.03818874, 0.09490126, 0.08855148, 0.2125093, 0.08227557, 0.08470487, 0.209201, 0.2582166, 0.2574722, 0.1789012, 0.08130053, 0.1679286, 0.160275, 0.2448826, 0.3311343, 0.1758152, 0.2415201, 0.2255063, 0.1560306, 0.4416802, 0.1643404, 0.1457635, 0.2772573, 0.2048854, 0.2960801, 0.2476019, 0.2695129, 0.2244126, 0.2957559, 0.4200395, 0.2259825, 0.2407729, 0.3770088, 0.3665841, 0.3029803, 0.2056194, 0.3520936, 0.2167107, 0.2046959, 0.3291155, 0.3910464, 0.1646348, 0.2165021, 0.3346155, 0.2262629, 0.2084854, 0.2110836, 0.2591557, 0.2008202, 0.3206674, 0.1828827, 0.1678419, 0.1040793, 0.1215412, 0.08980383, 0.09788448, 0.1193961, 0.1202668, 0.2274841, 0.06120391, 0.1093182, 0.07534064, 0.03063882, 0.07211118, 0.05712711, 0.1114604, -0.07951922, 0.1396042, -0.01969447, 0.08401145, -0.0447249, -0.04265186, 0.02937593, 0.06205165, -0.06586276, -0.1753078, -0.1066748, -0.0673872, -0.1632187, -0.04192356, -0.1428787, -0.0426021, -0.2035975, -0.06565272, -0.04056716, -0.2647806, -0.1290269, -0.1322684, -0.2957015, -0.2877416, -0.2955568, -0.05648805, -0.234909, -0.2691437, -0.2996495, -0.1859454, -0.3274415, -0.3609194, -0.2918717, -0.3385775, -0.2196397, -0.2863275, -0.2394704, -0.3640491, -0.3170604, -0.2901381, -0.2282063, -0.2051259, -0.1936819, -0.2955973, -0.3144498, -0.3821933, -0.2391596, -0.433311, -0.3796214, -0.314876, -0.3221517, -0.3894935, -0.1476328, -0.2084545, -0.1951183, -0.2783258, -0.2985663, -0.208764, -0.2376565, -0.08466602, -0.227661, -0.3256213, -0.1248845, -0.2117993, 0.00437712, -0.1349414, -0.2264118, -0.1842283, 0.003225805, -0.1287456, -0.2332626, -0.04535669, -0.03814603, 0.01720968, -0.02541079, -0.08681296, -0.0001985755, -0.01050433,  };

BLA::Matrix<Nobs> obs; // observation vector
KALMAN<Nstate,Nobs> K; // your Kalman filter
BLA::Matrix<Nstate> state;

Variometer variometer = Variometer();

void setup() 
{
  Serial.begin(115200);
  Serial.println("Begun");

  // time evolution matrix
  K.F = {1.0, 0.0, 0.0,
          0.0, 1.0, 0.0,
          0.0, 0.0, 1.0};

  // measurement matrix
  K.H = {1.0, 0.0, 0.0,
          0.0, 0.0, 1.0};
  // measurement covariance matrix
  K.R = {n_p*n_p,   0.0,
          0.0, n_a*n_a};
  // model covariance matrix
  K.Q = {m_p*m_p,     0.0,     0.0,
              0.0, m_s*m_s,     0.0,
              0.0,     0.0, m_a*m_a};
  
  
  state.Fill(0.0);
  obs.Fill(0.0);
  Serial.println("Here ok wtf");
}