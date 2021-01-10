#include <iostream>
#include <vector>
#include <numeric>
#include <fstream>
#include <chrono>
#include <random>
#include <list>
#include <iomanip>
#include <cmath>
#include <stdlib.h>
#include <ctime>

//Proportional Response Dynamics

// returned:
// - Equilibrium prices
// - equilibrium allocations

//Paper: Distributed Algorithms via Gradient Descent for Fisher Markets

//floor function eingefügt

//TODO:
// der Bidder mit der größten Val eines Guts, bekommt es zugewiesen, sofern er es sich leisten kann!
// (beginnend beim 1. Bidder)

/*
BSP:

Höchste Valuation pro Gut (Achtung: Bidder werden ab 0 gezählt)
(21,1) | (23,3) | (13,3) | (25,1) | (18,3) | (19,3) |


Optimale Allokation:
0 0 0 0 2.47 3  | 3 0 0 1.25 0 0  | 0 0 3 1.75 0 0  | 0 3 0 0 0.533 0  |

Update integrale (gerundete) Allokation:
0 0 0 0 2 3  | 3 0 0 1 0 0  | 0 0 3 1 0 0  | 0 3 0 0 0 0  |

*/




using namespace std;

class Bidder {
public:
    vector<double> valuation; //was mir ein gut wert ist
    double budget;
    vector<double> spent; //für welches gut gibt bidder was aus (summe aller elem in spent ist budget)

    friend ostream &operator<<(ostream &os, const Bidder &b);
};

ostream &operator<<(ostream &os, const Bidder &b) {
    for (int j = 0; j < b.spent.size(); ++j) {
        os << b.spent[j] << " ";
    }
    return os;
}

int random_number(int lb, int ub) {
    static unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    static std::mt19937 engine(seed);
    return (engine() % (ub - lb + 1)) + lb;
}

//Main method
int main() {


    auto start = std::chrono::system_clock::now();

    /*
   *
   *
   * HIER GEBEN WIR DIE EIGENSCHAFTEN DES EXPERIMENTS AN
   *
   *
   */

    //generate #goods
    int num_goods;
    cout << "Number Goods: ";
    cin >> num_goods;
    //int num_goods = 6;


    //vector<Bidder> bidders(5);

    //multiplier for valuation
    double i = 1.;

    //generate bidders with val, budget and spent_vec randomly
    int num_bidders;
    cout << "Number Bidders: ";
    cin >> num_bidders;
    //int num_bidders = 10;

    /*if (num_bidders > num_goods) {
        printf("Error number bidders larger than number goods");
        exit(EXIT_FAILURE);
    }*/

    //Quantität pro Gut
    double quantItem;
    cout << "Quantität eines Guts: ";
    cin >> quantItem;

    //num_iterations = Anzahl der Iterationen des Handels auf dem FM
    int num_iterations;
    cout << "Number Iterations: ";
    cin >> num_iterations;

    /*//Name der file, in die geschrieben wird
    string filename;
    cout << "Welchen Namen soll die File haben? ";
    cin >> filename;*/


    //num_iter_exp = Anzahl Ausführungen des Gesamtexperiments
    int num_iter_exp;
    cout << "Number Iterations Experiment: ";
    cin >> num_iter_exp;


    ofstream myfile2;
    myfile2.open("result.txt", std::ios_base::app);

    myfile2 << "Number Goods: " << num_goods << ", " << " Number Bidders: " << num_bidders << ", "
            << " Number Iterations: " << num_iterations << ", " << " Quantity per item: " << quantItem << "\n";
    myfile2 << "bidder budgets: " << "\n";

    //FOR SCHLEIFE FÜR ANZAHL WIEDERHOLUNGEN DES GESAMTEXPERIMENTS
    for (int iter = 1; iter < num_iter_exp; iter++) {


        vector<Bidder> bidders(num_bidders);

        for (int k = 0; k < num_bidders; ++k) {
            bidders[k].valuation.resize(num_goods);
            //valuation pro Gut und Bidder
            for (auto &v: bidders[k].valuation) v = (random_number(1, 11) + random_number(1, 15)) * i;

            //attention: was passiert wenn ein bidder nicht genug budget bekommt um sich alle Güter zu leisten?
            if(k == 1){
                bidders[k].budget = 2;
            }
            else{
                bidders[k].budget = random_number(1, 11) + random_number(1, 31);
            }

            //attention: keine beschränkung des budgets
            //bidders[k].budget = random_number(1, 11) + random_number(1, 31);

            myfile2 << bidders[k].budget << " | ";

            bidders[k].spent.resize(num_goods, bidders[0].budget / (double) num_goods);
        }

        myfile2 << "\n";


        //Attention: PR Dynamics wird hier berechnet
        vector<double> prices(num_goods);
        for (int it = 0; it < num_iterations; ++it) {

            //in jeder iteration werden die preise des guts i auf die menge der preise,
            // die jeder bidder ausgegeben hat, gesetzt
            for (int j = 0; j < num_goods; ++j) {
                prices[j] = 0;
                for (int i = 0; i < bidders.size(); ++i)
                    prices[j] += bidders[i].spent[j];

            }
            //update der valuations und spents pro bidder
            vector<vector<double>> update(bidders.size(), vector<double>(num_goods)); //
            for (int i = 0; i < bidders.size(); ++i) {
                for (int j = 0; j < num_goods; ++j) {
                    update[i][j] = bidders[i].valuation[j] * bidders[i].spent[j] / prices[j];

                }
            }

            //new bid vector for next iteration
            for (int i = 0; i < bidders.size(); ++i) {
                for (int j = 0; j < num_goods; ++j) {
                    bidders[i].spent[j] =
                            bidders[i].budget * update[i][j] / accumulate(update[i].begin(), update[i].end(), 0.0);

                }
            }

            //print für jeden bidder und jede iteration dessen ???
            cout << "Iteration " << it << ":\n";
            for (int i = 0; i < bidders.size(); ++i) {
                cout << "Bidder " << i << ": " << bidders[i] << endl;
            }
            cout << endl;

        }




        //set precision
        int pre = 3;




        // Kantengewichte sind die Allokationen der Güter auf die Bieter!!
        /*** Write allocations to graph ***/

        vector<vector<double>> graph(num_bidders, vector<double>(num_goods));
        for (int i = 0; i < num_bidders; ++i) {
            for (int j = 0; j < num_goods; ++j) {

                double allocItem = bidders[i].spent[j] / prices[j];

                //attention: jetzt gibt es keine Werte unter 0.01 und übe
                if (allocItem <= 0.01) {
                    allocItem = 0.0;
                }

                if (allocItem > 1) {
                    allocItem = 1.0;
                }

                graph[i][j] = allocItem;
            }
        }


        //das ist graph * quant pro gut und bidder
        vector<vector<double>> allocQuant(num_bidders, vector<double>(num_goods));



        //attention: berechnen frac part für Gut über alle bidder
        vector<double> fracVec(num_goods);

        for (int i = 0; i < num_bidders; ++i) {
            for (int j = 0; j < num_goods; ++j) {

                allocQuant[i][j] = quantItem * graph[i][j];

                if (allocQuant[i][j] < 0.001) {
                    graph[i][j] = 0;
                }

                fracVec[j] += allocQuant[i][j] - floor(allocQuant[i][j]);

            }
        }




        //sortiere höchste valuation für jeweiliges gut raus
        cout << "\n";
        cout << "Höchste Valuation pro Gut (Achtung: Bidder werden ab 0 gezählt) \n ";

        vector<pair<int, int> > vecPair(num_goods);

        //höchste Valuation für jeweils ein Gut = Entscheidung wem die fraktionalen Teilen eines Guts
        // zugewiesen werden

        int greatest_val = 0;


        //attention: berechnen hier die höchste valuation für ein gut (und den dazugehörigen bidder)
        for (int i = 0; i < num_goods; ++i) {
            for (int b = 0; b < num_bidders; ++b) {
                if (bidders[b].valuation[i] >= greatest_val) {
                    greatest_val = bidders[b].valuation[i];
                    vecPair[i] = make_pair(greatest_val, b);
                }

            }
            cout << "(" << vecPair[i].first << "," << vecPair[i].second << ")";
            cout << " | ";
            greatest_val = 0;
        }
        cout << endl;



        //Attention: Initialisiere mit integraler Allokation
        /*up_integral := die integralen Ergebnisse, nachdem die fraktionalen Teile den jeweiligen Biddern
        zugeteilt wurden */

        vector<vector<double>> up_integral(num_bidders, vector<double>(num_goods));


        for (int i = 0; i < num_bidders; ++i) {
            for (int j = 0; j < num_goods; ++j) {

                up_integral[i][j] = floor(allocQuant[i][j]);

                //attention: weise summe fraktionaler teile dem bidder mit höchster valuation zu (für bestimments gut)

                if ((vecPair[j].second) == i && bidders[i].budget >= fracVec[j] * prices[j]) {
                   // up_integral[i][j] += quantItem * fracVec[j];
                    up_integral[i][j] += fracVec[j];
                    bidders[i].budget -= fracVec[j] * prices[j];
                }


            }
        }

        cout << "\n";
        cout << "\n";
        cout << "Optimale Allokation: \n";
        /*** print graph ***/
        for (int i = 0; i < num_bidders; ++i) {
            for (int j = 0; j < num_goods; ++j) {
                if (allocQuant[i][j] < 0.001) {
                    graph[i][j] = 0;
                }
                cout << allocQuant[i][j] << " ";
            }
            cout << " | ";
        }
        cout << "\n";

        //neue Allokation nach dem Runden der Allokationen:
        cout << "\n";
        cout << "Update integrale (gerundete) Allokation: \n";
        for (int i = 0; i < num_bidders; ++i) {
            for (int j = 0; j < num_goods; ++j) {
                cout << up_integral[i][j] << " ";
            }
            cout << " | ";
        }
        cout << endl;



        vector<double> max_utility(num_bidders);

        for (int i = 0; i < num_bidders; ++i) {
            for (int j = 0; j < num_goods; ++j) {

                max_utility[i] += allocQuant[i][j] * bidders[i].valuation[j];

            }
        }


    /*    //for debugging
        for (int i = 0; i < num_bidders; ++i) {
            cout << "Max Utility: " << max_utility[i] << std::setprecision(pre) << endl;
        }
*/

        vector<double> rd_max_utility(num_bidders);
        double rd_util;

        for (int i = 0; i < num_bidders; ++i) {
            rd_util = 0.0;
            for (int j = 0; j < num_goods; ++j) {
                rd_util += (up_integral[i][j] * bidders[i].valuation[j]);
            }
            rd_max_utility[i] = rd_util;
        }


        cout << "\n";

        if (iter == (num_iter_exp - 1)) {

            myfile2 << "rd max util " << " | " << "max_utility" << "\n";
            cout << "rd max util " << " | " << "max_utility" << "\n";

            for (int i = 0; i < num_bidders; ++i) {
                myfile2 << rd_max_utility[i] << " | " << max_utility[i] << "\n";
                cout << rd_max_utility[i] << " | " << max_utility[i] << "\n";

            }

            myfile2 << "\n";
            cout << "\n";

        }


        auto end = std::chrono::system_clock::now();

        std::chrono::duration<double> elapsed_seconds = end - start;
        std::time_t end_time = std::chrono::system_clock::to_time_t(end);

        if (iter == (num_iter_exp - 1)) {
            cout << "finished computation at " << std::ctime(&end_time)
                 << "elapsed time: " << elapsed_seconds.count() << "s\n";
        }


    }


    //TODO: es werden mehr Güter, als es nach Quantität gibt, verteilt.



    return 0;


}