#include "string.h"
#include <cmath>
#include "mg.h"
#include <iostream>
#include "fmg.h"
//#include "colorize_emxutil.h"

#include "bitmap.h"

void getvolcolor(ImageArray *pvar0, ImageArray *pvar1, double pvar6, double pvar7, ImageArray *n0)
{

	int size1 = pvar1->height;
	int size0 = pvar1->width;
	int sizel;
	sizel = 3;
	const int n = size1;
	const int m = size0;
	const int k = 1;
	int max_d, max_d1, max_d2, in_itr_num, out_itr_num, itr;
	int x, y, z;
	//    if (sizel>3){
	//        k=sizes[3];
	//    } else {
	//        k=1;
	//    }
	max_d1 = int(floor(log(n) / log(2) - 2) + 0.1);
	max_d2 = int(floor(log(m) / log(2) - 2) + 0.1);
	if (max_d1>max_d2){
		max_d = max_d2;
	}
	else{
		max_d = max_d1;
	}
	double *lblImg_pr, *img_pr;
	double *res_pr;
	double **res_prv;
	double *dx_pr, *dy_pr, *idx_pr, *idy_pr;
	lblImg_pr = pvar0->data;
	img_pr = pvar1->data;

	Tensor3d D, G, I;
	Tensor3d Dx, Dy, iDx, iDy;
	MG smk;
	G.set(m, n, k);
	D.set(m, n, k);
	I.set(m, n, k);
	dy_pr = 0;
	dx_pr = 0;
	idy_pr = 0;
	idx_pr = 0;
	//  if (nrhs>6){
	in_itr_num = int(pvar6 + 0.5);
	//  }else{
	//    in_itr_num=5;
	//  }
	//  if (nrhs>7){
	out_itr_num = int(pvar7 + 0.5);
	//  }else{
	//    out_itr_num=2;
	//  }

	Dx.set(m, n, k - 1);
	Dy.set(m, n, k - 1);
	iDx.set(m, n, k - 1);
	iDy.set(m, n, k - 1);
	for (z = 0; z<(k - 1); z++){
		for (y = 0; y<n; y++){
			for (x = 0; x<m; x++){
				Dx(x, y, z) = *dx_pr; dx_pr++;
				Dy(x, y, z) = *dy_pr; dy_pr++;
				iDx(x, y, z) = *idx_pr; idx_pr++;
				iDy(x, y, z) = *idy_pr; idy_pr++;
			}
		}
	}
	//    int dims[4];
	//    dims[0]=m; dims[1]=n; dims[2]=3; dims[3]=k;
	//double lvar0[m][n][3][k];//=mxCreateNumericArray(4, dims, mxDOUBLE_CLASS, mxREAL);
	res_pr = n0->data;
#if 0
	res_prv = new double*[k];
#else
	double* res_prvA[1];
	res_prv = &(res_prvA[0]);
#endif
	for (z = 0; z<k; z++){
		res_prv[z] = res_pr + n*m * 3 * z;
	}

	for (z = 0; z<k; z++){
		for (y = 0; y<n; y++){
			for (x = 0; x<m; x++){
				I(x, y, z) = lblImg_pr[x + m*y + z*n*m];
				G(x, y, z) = img_pr[x + y*m + z*m*n * 3];
				I(x, y, z) = !I(x, y, z);
			}
		}
	}
	for (z = 0; z<k; z++){
		for (y = 0; y<n; y++){
			for (x = 0; x<m; x++){
				(*res_prv[z]) = G(x, y, z);
				res_prv[z]++;
			}
		}
	}
	smk.set(m, n, k, max_d);
	smk.setI(I);
	smk.setG(G);
	smk.setFlow(Dx, Dy, iDx, iDy);
	for (int t = 1; t<3; t++){
		for (z = 0; z<k; z++){
			for (y = 0; y<n; y++){
				for (x = 0; x<m; x++){
					D(x, y, z) = img_pr[x + y*m + n*m*t + z*m*n * 3];
					smk.P()(x, y, z) = img_pr[x + y*m + n*m*t + z*m*n * 3];
					D(x, y, z) *= (!I(x, y, z));
				}
			}
		}
		smk.Div() = D;
		Tensor3d tP2;
		if (k == 1){
			for (itr = 0; itr<out_itr_num; itr++){
				smk.setDepth(max_d);
				Field_MGN(&smk, in_itr_num, 2);
				smk.setDepth(ceil(max_d / 2));
				Field_MGN(&smk, in_itr_num, 2);
				smk.setDepth(2);
				Field_MGN(&smk, in_itr_num, 2);
				smk.setDepth(1);
				Field_MGN(&smk, in_itr_num, 4);
			}
		}
		else{
			for (itr = 0; itr<out_itr_num; itr++){
				smk.setDepth(2);
				Field_MGN(&smk, in_itr_num, 2);
				smk.setDepth(1);
				Field_MGN(&smk, in_itr_num, 4);
			}
		}
		tP2 = smk.P();
		for (z = 0; z<k; z++){
			for (y = 0; y<n; y++){
				for (x = 0; x<m; x++){
					(*res_prv[z]) = tP2(x, y, z);
					res_prv[z]++;
				}
			}
		}
	}
}
