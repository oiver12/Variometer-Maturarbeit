#include "LinearRegression.h"
#include "Variometer.h"
#include "FiFo.h"

#include <Kalman.h>
using namespace BLA;

#define Nstate 3 // position, speed, acceleration
#define Nobs 2   // position, acceleration

// measurement std
#define n_p 0.1298
#define n_a 0.0069
//Process Noise
#define n_r_a 0.3

//vom C# Projekt 
double times[400] = { 0, 0.04, 0.08, 0.12, 0.16, 0.2, 0.24, 0.28, 0.32, 0.36, 0.4, 0.44, 0.48, 0.52, 0.56, 0.6, 0.64, 0.68, 0.72, 0.76, 0.8, 0.84, 0.88, 0.92, 0.96, 1, 1.04, 1.08, 1.12, 1.16, 1.2, 1.24, 1.28, 1.32, 1.36, 1.4, 1.44, 1.48, 1.52, 1.56, 1.6, 1.64, 1.68, 1.72, 1.76, 1.8, 1.84, 1.88, 1.92, 1.96, 2, 2.04, 2.08, 2.12, 2.16, 2.2, 2.24, 2.28, 2.32, 2.36, 2.4, 2.44, 2.48, 2.52, 2.56, 2.6, 2.64, 2.68, 2.72, 2.76, 2.8, 2.84, 2.88, 2.92, 2.96, 3, 3.04, 3.08, 3.12, 3.16, 3.2, 3.24, 3.28, 3.32, 3.36, 3.4, 3.44, 3.48, 3.52, 3.56, 3.6, 3.64, 3.68, 3.72, 3.76, 3.8, 3.84, 3.88, 3.92, 3.96, 4, 4.04, 4.08, 4.12, 4.16, 4.2, 4.24, 4.28, 4.32, 4.36, 4.4, 4.44, 4.48, 4.52, 4.56, 4.6, 4.64, 4.68, 4.72, 4.76, 4.8, 4.84, 4.88, 4.92, 4.96, 5, 5.04, 5.08, 5.12, 5.16, 5.2, 5.24, 5.28, 5.32, 5.36, 5.4, 5.44, 5.48, 5.52, 5.56, 5.6, 5.64, 5.68, 5.72, 5.76, 5.8, 5.84, 5.88, 5.92, 5.96, 6, 6.04, 6.08, 6.12, 6.16, 6.2, 6.24, 6.28, 6.32, 6.36, 6.4, 6.44, 6.48, 6.52, 6.56, 6.6, 6.64, 6.68, 6.72, 6.76, 6.8, 6.84, 6.88, 6.92, 6.96, 7, 7.04, 7.08, 7.12, 7.16, 7.2, 7.24, 7.28, 7.32, 7.36, 7.4, 7.44, 7.48, 7.52, 7.56, 7.6, 7.64, 7.68, 7.72, 7.76, 7.8, 7.84, 7.88, 7.92, 7.96, 8, 8.04, 8.08, 8.12, 8.16, 8.2, 8.24, 8.28, 8.32, 8.36, 8.4, 8.44, 8.48, 8.52, 8.559999, 8.599999, 8.639999, 8.679999, 8.72, 8.76, 8.8, 8.84, 8.88, 8.92, 8.96, 9, 9.04, 9.08, 9.12, 9.16, 9.2, 9.24, 9.28, 9.32, 9.36, 9.4, 9.44, 9.48, 9.52, 9.559999, 9.599999, 9.639999, 9.679999, 9.719999, 9.76, 9.8, 9.84, 9.88, 9.92, 9.96, 10, 10.04, 10.08, 10.12, 10.16, 10.2, 10.24, 10.28, 10.32, 10.36, 10.4, 10.44, 10.48, 10.52, 10.56, 10.6, 10.64, 10.68, 10.72, 10.76, 10.8, 10.84, 10.88, 10.92, 10.96, 11, 11.04, 11.08, 11.12, 11.16, 11.2, 11.24, 11.28, 11.32, 11.36, 11.4, 11.44, 11.48, 11.52, 11.56, 11.6, 11.64, 11.68, 11.72, 11.76, 11.8, 11.84, 11.88, 11.92, 11.96, 12, 12.04, 12.08, 12.12, 12.16, 12.2, 12.24, 12.28, 12.32, 12.36, 12.4, 12.44, 12.48, 12.52, 12.56, 12.6, 12.64, 12.68, 12.72, 12.76, 12.8, 12.84, 12.88, 12.92, 12.96, 13, 13.04, 13.08, 13.12, 13.16, 13.2, 13.24, 13.28, 13.32, 13.36, 13.4, 13.44, 13.48, 13.52, 13.56, 13.6, 13.64, 13.68, 13.72, 13.76, 13.8, 13.84, 13.88, 13.92, 13.96, 14, 14.04, 14.08, 14.12, 14.16, 14.2, 14.24, 14.28, 14.32, 14.36, 14.4, 14.44, 14.48, 14.52, 14.56, 14.6, 14.64, 14.68, 14.72, 14.76, 14.8, 14.84, 14.88, 14.92, 14.96, 15, 15.04, 15.08, 15.12, 15.16, 15.2, 15.24, 15.28, 15.32, 15.36, 15.4, 15.44, 15.48, 15.52, 15.56, 15.6, 15.64, 15.68, 15.72, 15.76, 15.8, 15.84, 15.88, 15.92, 15.96};
double measHeights[400] = { -0.02912948, -0.1105125, 0.2966188, -0.0318943, 0.359989, 0.1143814, -0.04417511, 0.2844964, -0.02225279, 0.1085933, 0.2448583, 0.01672431, 0.2814505, 0.04245853, 0.2887411, 0.117452, 0.1094433, 0.03820059, 0.2083575, 0.2525993, 0.1746306, 0.2752374, 0.1299701, 0.5224944, 0.2931299, 0.2768311, 0.2814109, 0.2615755, 0.2739499, 0.3809986, 0.3209375, 0.2320855, 0.4459167, 0.4218712, 0.555883, 0.3767521, 0.5786897, 0.5369079, 0.2463205, 0.6970103, 0.5374006, 0.5214585, 0.6010708, 0.6996937, 0.5868252, 0.4877449, 0.610286, 0.7162175, 0.6476284, 0.8033948, 0.756234, 0.8143196, 0.8547163, 1.072633, 0.7613301, 1.113559, 0.9661927, 1.255403, 0.9589812, 0.9167666, 0.9330981, 1.019998, 1.056465, 0.973849, 1.211156, 0.9845583, 1.115176, 1.280434, 1.363976, 1.157811, 1.381025, 1.223287, 1.47623, 1.40201, 1.235378, 1.571864, 1.408479, 1.514828, 1.516136, 1.701581, 1.662824, 1.824958, 1.512979, 1.669809, 1.568156, 1.598457, 1.948566, 1.731273, 1.743116, 2.163883, 1.844287, 1.77991, 1.918912, 2.340014, 1.970887, 2.178588, 2.170439, 2.071173, 1.983883, 2.130468, 2.186978, 2.448613, 2.131646, 2.208049, 2.180324, 2.246653, 2.112001, 2.281063, 2.571684, 2.154007, 2.412285, 2.471734, 2.60901, 2.360984, 2.725874, 2.300173, 2.443339, 2.564386, 2.426254, 2.762749, 2.708945, 2.683612, 2.609115, 2.833038, 2.463223, 2.738982, 2.70593, 2.845188, 2.777398, 2.821545, 2.648398, 2.931176, 2.98469, 3.013425, 2.973812, 3.069943, 2.690623, 2.916464, 3.274207, 2.855385, 2.92077, 2.920738, 3.165908, 3.065642, 3.046323, 3.036516, 3.385986, 3.051398, 2.998906, 3.150638, 3.003476, 2.942446, 3.071409, 3.079604, 2.937087, 3.26135, 3.164551, 3.057935, 3.136339, 3.222583, 3.265752, 3.302684, 3.185485, 3.069813, 3.270587, 3.168754, 3.142673, 3.198781, 3.336258, 3.198437, 3.294785, 3.351468, 3.437927, 3.37291, 3.61731, 3.223823, 3.216007, 3.239293, 3.307268, 3.352751, 3.263113, 3.344413, 3.260982, 3.410638, 3.341328, 3.492364, 3.600097, 3.532109, 3.773349, 3.510481, 3.676572, 3.567019, 3.592927, 3.677712, 3.414342, 3.796469, 3.591406, 3.769692, 3.783421, 3.799577, 3.497044, 3.86561, 3.817334, 3.825857, 3.708641, 3.804368, 3.774268, 3.959836, 4.015144, 4.087921, 3.862334, 3.900385, 4.095072, 4.050343, 4.255749, 3.994282, 4.048046, 4.087185, 4.32507, 4.081381, 4.09678, 4.305202, 4.331569, 4.402811, 4.366507, 4.42787, 4.60315, 4.56242, 4.332485, 4.546312, 4.46034, 4.482335, 4.784163, 4.618055, 4.430904, 4.79585, 4.765206, 4.913724, 5.015034, 4.537457, 4.951478, 4.919865, 5.02881, 4.809126, 4.954278, 5.063263, 5.254391, 5.035386, 5.074378, 5.077269, 5.12875, 5.220941, 5.281103, 5.19591, 5.435675, 5.235534, 5.108996, 5.411129, 5.507104, 5.071058, 5.417593, 5.6726, 5.359987, 5.370877, 5.296946, 5.505812, 5.488845, 5.66057, 5.907876, 5.778223, 5.717286, 5.424376, 5.747224, 5.817279, 5.762604, 5.90593, 5.804861, 5.706352, 6.027907, 5.974368, 5.897181, 5.785001, 5.779659, 5.877512, 5.737586, 6.082582, 6.13717, 5.899219, 6.086234, 5.988482, 6.000278, 6.002359, 5.976665, 6.176499, 6.205866, 6.117873, 6.246521, 6.207196, 6.153632, 6.143867, 6.036177, 6.138185, 6.005884, 6.358092, 5.99851, 6.231661, 6.594237, 6.527777, 6.163552, 6.314048, 6.182212, 6.296288, 6.151064, 6.152391, 6.325093, 6.235705, 6.517571, 6.294614, 6.437672, 6.396879, 6.239766, 6.288031, 6.226674, 6.187889, 6.585267, 6.595814, 6.616472, 6.472719, 6.471117, 6.535197, 6.591656, 6.602948, 6.545747, 6.469813, 6.489776, 6.49987, 6.500508, 6.643331, 6.48033, 6.84382, 6.420943, 6.456125, 6.820229, 6.675767, 6.774029, 6.695356, 6.762586, 6.670973, 6.78445, 6.774082, 6.577893, 7.092179, 6.945987, 6.804735, 7.000465, 6.969766, 6.981452, 7.090513, 6.670312, 6.998944, 6.851528, 6.915498, 7.211845, 7.277596, 7.088304, 7.149361, 7.054083, 7.2176, 7.283465, 7.492973, 7.266426, 7.263334, 7.164162, 7.239814, 7.365232, 7.395138, 7.278498, 7.320364, 7.554478, 7.468184, 7.416534, 7.529586, 7.579891, 7.604979, 7.717901, 7.58237, 7.716133, 7.590359, 7.54892, 7.388172, 7.697037, 7.850387, 7.796708, 7.843051, 7.779289, 7.824834, 7.878613, 8.015529, 8.154523, 8.008489};
double measAccelerations[400] = { 0.003142748, 0.001054991, 0.01683914, 0.03230924, 0.03377156, 0.04900306, 0.05142043, 0.06851383, 0.07897287, 0.0916447, 0.09925307, 0.1142452, 0.1167768, 0.1200243, 0.1252812, 0.1385529, 0.1487339, 0.1576706, 0.1668405, 0.171689, 0.179205, 0.1708217, 0.1900122, 0.1870423, 0.2002846, 0.2066914, 0.2138211, 0.2249152, 0.2255138, 0.2313395, 0.2362511, 0.2288063, 0.2407772, 0.2478091, 0.2345985, 0.2461907, 0.2458992, 0.2571865, 0.2547489, 0.2525434, 0.258395, 0.2477374, 0.2536366, 0.2414841, 0.248554, 0.258684, 0.2428017, 0.2391278, 0.2382737, 0.2398688, 0.2339645, 0.2341253, 0.2102775, 0.2005206, 0.2035549, 0.2005817, 0.1891403, 0.193016, 0.1831488, 0.176366, 0.1690945, 0.1582245, 0.1442147, 0.1391507, 0.1348688, 0.1265043, 0.1179254, 0.1212892, 0.1071776, 0.09544212, 0.08027481, 0.07723201, 0.06272645, 0.06320575, 0.04242916, 0.04286683, 0.02202548, 0.02222682, -0.003637154, 0.007817794, -0.0251415, -0.02072168, -0.04559058, -0.04978534, -0.06118719, -0.05788968, -0.0741529, -0.0849607, -0.08791736, -0.09646579, -0.1017991, -0.11923, -0.1263485, -0.136821, -0.1537289, -0.1620119, -0.153857, -0.1795691, -0.1735896, -0.188888, -0.1773987, -0.2092897, -0.1864922, -0.2075976, -0.2064349, -0.2130604, -0.2287349, -0.213581, -0.2345991, -0.2323825, -0.2309438, -0.2469999, -0.2560736, -0.2421176, -0.2417605, -0.2419412, -0.2534677, -0.2613194, -0.2369072, -0.2495991, -0.245365, -0.2245119, -0.2446706, -0.2478185, -0.2348258, -0.2366117, -0.2397874, -0.2265007, -0.2322238, -0.2300472, -0.2238913, -0.212191, -0.2100091, -0.2085846, -0.1881562, -0.1909954, -0.1847032, -0.1904051, -0.1838093, -0.1489186, -0.1580307, -0.1479396, -0.134446, -0.1357314, -0.1162304, -0.1254696, -0.1044859, -0.09200736, -0.0914386, -0.07603979, -0.06748869, -0.05103793, -0.05902826, -0.03084814, -0.007667847, -0.01255258, 0.002052283, -0.01047626, 0.01145312, 0.02218326, 0.03536554, 0.04901878, 0.04704284, 0.05850829, 0.07179783, 0.08142754, 0.08815355, 0.09504174, 0.1105519, 0.1257515, 0.1217019, 0.1365887, 0.1381828, 0.1377017, 0.1575081, 0.1524477, 0.1716022, 0.1763627, 0.1817511, 0.1967113, 0.1876375, 0.2069204, 0.2159506, 0.2109085, 0.2025672, 0.2244118, 0.227172, 0.2259725, 0.2438218, 0.2257688, 0.2396186, 0.2451879, 0.2455516, 0.253219, 0.2513022, 0.252439, 0.2470633, 0.2573918, 0.2468725, 0.2382938, 0.2423649, 0.2463708, 0.234439, 0.2370318, 0.2498154, 0.2351779, 0.2283593, 0.2280782, 0.2170644, 0.2191145, 0.2173238, 0.2041734, 0.2120491, 0.2075585, 0.2044687, 0.1849219, 0.1793259, 0.1732534, 0.164054, 0.1512394, 0.1456436, 0.1293387, 0.1317872, 0.1198319, 0.1078167, 0.09804995, 0.09627508, 0.07879889, 0.08980034, 0.06287955, 0.04722277, 0.04495451, 0.03978118, 0.01248899, 0.02863348, 0.0009415316, -0.00407515, -0.01451683, -0.04272453, -0.02337808, -0.02896067, -0.0524947, -0.06441705, -0.07939809, -0.09282987, -0.08844359, -0.09957075, -0.1116041, -0.1175822, -0.1138847, -0.1353659, -0.1359911, -0.1516441, -0.1582593, -0.1631646, -0.174178, -0.1860076, -0.2004121, -0.1796311, -0.1967548, -0.2035175, -0.205651, -0.2240632, -0.2197197, -0.2214223, -0.2342014, -0.2345671, -0.2370288, -0.2378828, -0.2485612, -0.2506831, -0.2605015, -0.2511978, -0.2364974, -0.2348771, -0.2505133, -0.2353902, -0.2443826, -0.2504453, -0.247087, -0.248638, -0.2445849, -0.2446877, -0.2399631, -0.22876, -0.2444929, -0.2078288, -0.2135618, -0.2219046, -0.206591, -0.2168473, -0.2064196, -0.1857507, -0.2013488, -0.1927591, -0.1774577, -0.1716285, -0.1504973, -0.1551662, -0.1458682, -0.1282541, -0.124952, -0.1205572, -0.1058543, -0.09384667, -0.09460316, -0.07724445, -0.08547181, -0.06566972, -0.04316993, -0.02716235, -0.0312542, -0.01919303, -0.004907306, 0.005192368, 0.02954106, 0.01356553, 0.03248419, 0.04476727, 0.0660045, 0.056509, 0.08340658, 0.07448574, 0.09823591, 0.08553025, 0.1138204, 0.1173023, 0.1174733, 0.1322768, 0.14684, 0.1457808, 0.164426, 0.1496831, 0.181798, 0.1799292, 0.1975623, 0.1938614, 0.1935576, 0.2023324, 0.2070406, 0.2083648, 0.2057333, 0.2254989, 0.2116538, 0.2301999, 0.2244977, 0.2303982, 0.2496288, 0.2366362, 0.2596449, 0.2490028, 0.2548387, 0.2445913, 0.2457537, 0.2369335, 0.2411964, 0.2458767, 0.2420404, 0.2433649, 0.2345562, 0.2451416, 0.2567142, 0.2410696, 0.2276528, 0.2369136, 0.2162232, 0.2151105, 0.2140984, 0.2110911, 0.2045079, 0.2034566, 0.1804228, 0.1806694, 0.1710815, 0.167484, 0.1642951, 0.153439, 0.1429227, 0.145814, 0.1190805, 0.1110304, 0.09975181, 0.09870996, 0.08770949, 0.08584253, 0.08301639, 0.06741313, 0.05729631, 0.04252932, 0.03331174, 0.03851284, 0.01238531, 0.009290677, -0.0002044841, -0.01655823, -0.01360208, -0.03012825, -0.04773892, -0.06096747, -0.05960559};

BLA::Matrix<Nobs> obs;
KALMAN<Nstate,Nobs> K; 
BLA::Matrix<Nstate> state;

Variometer variometer = Variometer();

void setup() 
{   
        Serial.begin(115200);
        Serial.println("Begun");


        K.P = {500.0, 0.0, 0.0,
                0.0, 500.0, 0.0,
                0.0, 0.0, 500.0
        };
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
        K.Q = {0.0,     0.0,     0.0,
                        0.0, 0.0,     0.0,
                        0.0,     0.0, 0.0};
        
        K.x.Fill(0.0);
        state.Fill(0.0);
        obs.Fill(0.0);
        size_t n = sizeof(times) / sizeof(times[0]);
        /*Serial.print("double[] linearRegressionVel = new double[] {");
        for (size_t i = 0; i < n; i++)
        {
                variometer.addSample(measHeights[i], times[i]);
        }
        Serial.println(" };");*/
        float deltaT = times[1] - times[0];
         K.F = {1.0,  deltaT,  deltaT*deltaT/2,
		0.0, 1.0, deltaT,
                0.0, 0.0, 1.0};
        K.Q = {(deltaT*deltaT*deltaT*deltaT) / 4, (deltaT*deltaT*deltaT) / 3, (deltaT*deltaT) / 2,
                (deltaT*deltaT*deltaT) / 3, (deltaT*deltaT), deltaT,
                (deltaT*deltaT) / 2, deltaT, 1};
        K.Q *= n_r_a * n_r_a;

        Serial.print("double[] KalmanVel = new double[] {");
        for (size_t i = 0; i < n; i++)
        {
                state(0) = measHeights[i];
                state(2) = measAccelerations[i];
                obs = K.H * state;
                K.update(obs);
                Serial.print(String(K.x(1), 7) + ", ");
        }
        Serial.println(" };");
}

void loop()
{

}
