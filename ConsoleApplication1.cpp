// PROJET SALLE DE MARCHE : Algo de décision
// Code: J.L. Phung, L. Zoude
//

#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <string>
#include <omp.h>
#include <vector>
#include <tuple>
#include <filesystem>
using namespace std;

bool replace(string& str, const std::string& from, const std::string& to) {
	size_t start_pos = str.find(from);
	if (start_pos == std::string::npos)
		return false;
	str.replace(start_pos, from.length(), to);
	return true;
}

int main()
{
	// PARCOURS DU DOSSIER
	string path = "H:/Projet Salle de Marché/Actions/";
	vector<string> fichiers;
	for (const auto & entry : experimental::filesystem::directory_iterator(path)) {
		//cout << entry.path() << endl;
		fichiers.push_back(entry.path().string());
	}
	cout << fichiers.size() << endl;
	omp_set_num_threads(fichiers.size());

#pragma omp parallel
	{
		int thread_id = omp_get_thread_num();
		cout << "cimer chef ... " << thread_id << endl;
		string filename;
		filename = fichiers[thread_id];
		//Achat : id,nb achat,Max € a payer
		vector<tuple<int, int, int> > vente;

		//Achat : id,nb vente,min € vente
		vector<tuple<int, int, int> > achat;

		ifstream fichier(filename, ios::in);
		if (fichier)  // si l'ouverture a réussi
		{
			cout << "Lecture du fichier " << filename << endl;
			int id_client, av, qte, prix;
			fichier >> id_client >> av >> qte >> prix;
			while (!(fichier.eof())) {
				if (av == 1) {
					achat.push_back(make_tuple(id_client, qte, prix));
				}
				else {
					vente.push_back(make_tuple(id_client, qte, prix));
				}
				//cout << id_client << " " << av << " " << qte << " " << prix << endl;  // on l'affiche
				fichier >> id_client >> av >> qte >> prix;
			}
			fichier.close();  // on ferme le fichier
		}
		else  // sinon
			cerr << "Impossible d'ouvrir le fichier !" << endl;

		/// CODE JL
		int nb_VenteTot = 0;
		int nb_AchatTot = 0;
		for (int i = 0; i < vente.size(); i++) {
			nb_VenteTot += get<1>(vente[i]);
		}


		for (int i = 0; i < achat.size(); i++) {
			nb_AchatTot += get<1>(achat[i]);
		}

		//cout << "Total demandes de vente : " << nb_VenteTot << endl;
		//cout << "Total demandes d'achats : " << nb_AchatTot << endl;
		double rs_Achat, rs_Vente;


		if (nb_VenteTot < nb_AchatTot)
			rs_Achat = (double)nb_VenteTot / nb_AchatTot;
		else
			rs_Achat = 1.0;


		if (nb_VenteTot > nb_AchatTot)
			rs_Vente = (double)nb_AchatTot / nb_VenteTot;
		else
			rs_Vente = 1.0;


		vector<tuple<int, int> > repartition_achat, repartition_vente;


		for (int i = 0; i < achat.size(); i++) {
			double tmp = (double)get<1>(achat[i]) * rs_Achat;
			repartition_achat.push_back(make_tuple(get<0>(achat[i]), round(tmp)));
		}



		for (int i = 0; i < vente.size(); i++) {
			double tmp = (double)get<1>(vente[i]) * rs_Vente;
			//cout << "tmp: " << tmp << "\n";
			repartition_vente.push_back(make_tuple(get<0>(vente[i]), round(tmp)));
		}

		replace(filename, "Actions", "Resultats");

		ofstream sortie(filename);

		if (sortie)  // si l'ouverture a réussi
		{
			sortie << "Resultats : " << endl;
			for (int i = 0; i < repartition_achat.size(); i++) {
				sortie << "Le client n." << get<0>(repartition_achat[i]) << " achete " << get<1>(repartition_achat[i]) << " actions.\n";
			}

			for (int i = 0; i < repartition_vente.size(); i++) {
				sortie << "Le client n." << get<0>(repartition_vente[i]) << " vend " << get<1>(repartition_vente[i]) << " actions.\n";
			}
			sortie.close();  // on ferme le fichier
			cout << "Ecriture reussie dans " << filename << "." << endl;
		}
		else {// sinon
			cerr << "Impossible d'ouvrir le fichier !" << endl;
		}
	}
	system("pause");

	return 0;
}