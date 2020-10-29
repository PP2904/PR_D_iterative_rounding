#include <iostream>
#include <vector>
#include <numeric>
#include <fstream>
#include <chrono>
#include <random>


//Proportional Response Dynamics

// returned:
// - Equilibrium prices
// - equilibrium allocations

//Paper: Distributed Algorithms via Gradient Descent for Fisher Markets

//floor function eingefügt


// gibt es pro Gut genau eine Einheit??? -> Annahme ja! (22.10.20)

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
    return (engine() % (ub-lb+1))+lb;
}

//Main method
int main() {

    ofstream myfile;
    myfile.open ("markets.txt", std::ios_base::app);

    //generate #goods
    int num_goods = 4;


    //vector<Bidder> bidders(5);

    //multiplier for valuation
    double i = 1.;

    //generate bidders with val, budget and spent_vec randomly
    int num_bidders = 4;


    vector<Bidder> bidders(num_bidders);

    for (int k = 0; k < num_bidders; ++k) {
        bidders[k].valuation.resize(num_goods);
        //valuation pro Gut und Bidder
        for(auto &v: bidders[k].valuation) v = (random_number(0,11)+random_number(0,15))*i;
        bidders[k].budget = random_number(0,11)+random_number(0,31);
        bidders[k].spent.resize(num_goods, bidders[0].budget / (double) num_goods);
    }
    /*//srand ( time(NULL) );
    for (int k = 0; k < num_bidders; ++k) {
        //values have no meaning, just randomizing
        double r1 = random_number(0,11);
        double r2 = random_number(0,31);
        double r3 = random_number(0,8);

        bidders[k].valuation = {(r1+r2/r3)* i, (r1+r2/r3) * i, (r1+r2/r3) * i};
        bidders[k].budget = r1+r2;
        bidders[k].spent.resize(num_goods, bidders[0].budget / (double) num_goods);

    }
*/
    int num_iterations = 2000;
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

        //writing to txt file
        /*ofstream myfile;
        myfile.open ("markets.txt", std::ios_base::app);*/
        myfile << "Iteration " << it << ":\n";
        for (int i = 0; i < bidders.size(); ++i) {
            myfile << "Bidder " << i << ": " << bidders[i] << endl;
        }
        myfile << endl;
    }

    //von Max utility und utility (im equilibrium sind diese gleich)

    vector<double> utility(num_bidders);
    vector<double> max_utility(num_bidders);
    for(int b=0; b< num_bidders; ++b) {
        max_utility[b] = 0;
        for(int i=0; i < num_goods; ++i)  {
            utility[b] += bidders[b].valuation[i]*bidders[b].spent[i]/prices[i]; //Aufpassen wenn prices[i] = 0!
            if(max_utility[b] < bidders[b].valuation[i]/prices[i]) {
                max_utility[b] = bidders[b].valuation[i]/prices[i];
            }
        }

        max_utility[b] *= bidders[b].budget;
    }

    // save utility from start
    vector<double> val_start(num_bidders);
    for(int b=0; b< num_bidders; ++b) {
        for (int i = 0; i < num_goods; ++i) {
            val_start[b] = bidders[b].valuation[i];
        }
    }


    //ich benötige die utility zu Beginn des Programms, um diese am Ende für die gerundeten Kanten mal 1 zu nehmen
    // und mit der max_utility aus dem fraktionalen Teil zu Vergleichen

    for(int i=0; i < num_bidders; ++i) {
        cout << utility[i] << endl;
    }

    //supply umbennenen und dafür
    cout << endl;
    cout << "Fraktionales/optimales Ergebnis: ";
    cout << endl;
    for (int j = 0; j < num_goods; ++j) {
        double demand = 0;
        double supply = 0;
        for (int i = 0; i < bidders.size(); ++i) {
            demand += bidders[i].spent[j] / prices[j];
        }
        cout << "Demand: " << demand << endl;
        //cout << "Supply: " << prices[j] << endl;
    }

    //set precision
    int pre = 3;

    for(int i=0; i < num_bidders; ++i){
        cout << "Max Utility: " << max_utility[i] << std::setprecision(pre) << endl;
    }


    //                      //
    //                      //
    //                      //
    // writing to txt file  //
    //                      //
    //                      //
    //                      //

  /*  *//*ofstream myfile;
    myfile.open ("markets.txt", std::ios_base::app);*//*
    myfile << endl;
    myfile << "D/MaxUtil: " << endl;
    for (int j = 0; j < num_goods; ++j) {
        double demand = 0;
        double supply = 0;
        for (int i = 0; i < bidders.size(); ++i) {
            demand += bidders[i].spent[j] / prices[j];
            myfile << "Bidder " << i << " receives " << bidders[i].spent[j] / prices[j] << "of good " << j << endl;
        }
        myfile << "Demand: " << demand << endl;
        myfile << "\n";
    }



    myfile.close();*/

    /*** Write allocations to graph ***/
    vector<vector<double>> graph(num_bidders,vector<double>(num_goods));
    for(int i=0; i < num_bidders; ++i) {
        for(int j=0; j < num_goods; ++j) {
            graph[i][j] = bidders[i].spent[j] / prices[j];
        }
    }



    /*** hier können 2 bidder das gleiche Gut bekommen ***//*

    cout << "\n";
    for (int j = 0; j < num_goods; ++j) {
        for (int i = 0; i < num_bidders; ++i) {
            cout << 100*(graph[i][j]) << " ";
        }
    }*/


    /*** wir tauschen hier bidder und goods, sodass nicht 2 Bidder das gleiche Gut zugewiesen bekommen ***/

    /* Kantengewichte sind die Allokationen der Güter auf die Bieter!! nicht die valuations */

    /*multiplizieren die Allokation mit 20, da wir einen Wert > 1 erhalten wollen, um iterative Rounding
     * nutzen zu können */

    cout << "\n";
    cout << "Allokation/Kantengewicht: ";
    /*** print graph ***/
    for(int i=0; i < num_bidders; ++i) {
        for (int j=0; j < num_goods; ++j) {
            /*if(20*(graph[i][j]) < 1){
                graph[i][j] = 0;
            }*/
            cout << 20*(graph[i][j]) << " ";
        }
        cout << " | ";
    }
    cout << "\n";

    /*** runde ich die Nachkommastellen, während ich die Vorkommastelle behalte; ***/


    //fractional and integral parts

    cout << "\n";
    cout << "fractional Allokation/Kantengewicht: ";
    /*** print graph ***/
    for(int i=0; i < num_bidders; ++i) {
        for (int j=0; j < num_goods; ++j) {
            if(20*(graph[i][j]) < 1){
                graph[i][j] = 0;
            }
            cout << std::setprecision(pre) << (20*(graph[i][j])-floor(20*(graph[i][j]))) << " ";
        }
        cout << " | ";
    }


    cout << "\n";
    cout << "summe fractional ";
    for(int i=0; i < num_bidders; ++i) {
        for (int j=0; j < num_goods; ++j) {
            if(20*(graph[i][j]) < 1){
                graph[i][j] = 0;
            }
            if(graph[i][j]>1e-4){
                cout << std::setprecision(pre) << (20*(graph[i][j])-floor(20*(graph[i][j]))) << " ";
            }

        }

    }



    cout << "\n";

    cout << "\n";
    cout << "integral Allokation/Kantengewicht: ";
    /*** print graph ***/
    for(int i=0; i < num_bidders; ++i) {
        for (int j=0; j < num_goods; ++j) {
            cout << std::setprecision(pre) << floor(20*(graph[i][j])) << " ";
        }
        cout << " | ";
    }
    cout << "\n";

    cout << "\n";
    /* Kantengewichte sind die Allokationen der Güter auf die Bieter!! nicht die valuations */

    cout << "Valuation der Güter (jeweils für einen Bidder Gut 1 bis " << num_goods << "): \n ";
    for(int b=0; b< num_bidders; ++b) {
        for (int i = 0; i < num_goods; ++i) {
            cout << bidders[b].valuation[i] << " ";
        }
        cout << " | ";
    }

    /*//gibt mir nicht die initialen utility wert, sondern die vom ende (=max_utility) aus ?!
    for(int b=0; b< num_bidders; ++b) {
        cout << val_start[b] << endl;
    }*/


    cout << " \n ";
    cout << "Money spend for good: ";
    for(int b=0; b< num_bidders; ++b) {
        for (int i = 0; i < num_goods; ++i) {
            if(bidders[b].spent[i] < (10^-10)){
                bidders[b].spent[i] =0;
            }
            cout << std::fixed << (bidders[b].spent[i]) << " ";
        }
        cout << " | ";
    }


    cout << " \n ";
    cout << "Budget der Bidder: ";
    for(int b=0; b< num_bidders; ++b) {
        cout << (bidders[b].budget)<< " ";
    }


    cout << " \n ";
    cout << "Preise der Güter: ";
    for (int i = 0; i < num_goods; ++i) {
        cout << prices[i]<< " ";
    }

    for(int i=0; i < num_bidders; ++i) {
        for (int j=0; j < num_goods; ++j) {
            cout << std::setprecision(pre) << floor(20*(graph[i][j])) << " ";
        }
        cout << " | ";
    }
    cout << "\n";

    for(int i=0; i < num_bidders; ++i) {
        for (int j=0; j < num_goods; ++j) {
            cout << std::setprecision(pre) << floor(20*(graph[i][j])) << " ";
        }
    }







    /*** wie kann ich genau die Werte ausgeben der valuations, die ungleich 0 sind? ***//*
    cout << " \n ";
    cout << "Filter: ";
    for(int i=0; i < num_bidders; ++i) {
        for (int j=0; j < num_goods; ++j) {
            if (graph[i][j]==1) {
                cout << bidders[i].valuation[j] << " ";
            }
        }
    }
*/
    return 0;

}

