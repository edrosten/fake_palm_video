#include <cvd/image_io.h>
#include <cvd/colourmap.h>
#include <cvd/vision.h>
#include <cvd/videodisplay.h>
#include <cvd/convolution.h>
#include <cvd/gl_helpers.h>
#include <cvd/random.h>
#include <tag/printf.h>
#include <algorithm>

using namespace std;
using namespace CVD;
int main()
{

	//Image<Rgba<byte> > i = img_load("blue-logo_cleo_2015.bmp.png");
	//Image<Rgba<byte> > i = img_load("Kings-college-london-logo-2.png");
	Image<Rgba<byte> > i = img_load("512px-Meisje_met_de_parel.png");
	int mag=4;
	
	Image<float> pic(i.size()/mag + ImageRef(1,1), 0);
	Image<float> inc(i.size(), 0);
	vector<ImageRef> ps;

	for(ImageRef p(-1,0); p.next(i.size()); )
	{
		for(int j=0; j < i[p].red; j++)
		{
		//if(!i[p].red)
			ps.push_back(p);

			pic[p/mag] = 1;
		}
	}

	const float reconstruct_kernel = 1.;
	const float psf = 1.;
	const float noise=0.1;

	const float blur=8;

	convolveGaussian(pic, pic, blur);
		
	
	//Image<Rgb<byte> >  out(ImageRef(i.size().x, i.size().y*2));
	Image<Rgb<byte> >  out(ImageRef(i.size().x*2, i.size().y));
	VideoDisplay d(out.size());
	
	int os=0;

	for(int nn=0;nn<1000;nn++)
	{
		Image<float> f(i.size() / mag + ImageRef(1,1), 0);
		srand(1);
		vector<ImageRef> pts;
		inc.zero();
		//for(int i=0; i < nn+1; i++)

		int num = 10*pow(1.2, nn/20.);
		for(int i=0; i < num; i++)
		{
			ImageRef p = ps[rand()%ps.size()];
			pts.push_back(p);
			f[p/mag]++;
		//	inc[p]++;
		}

		for(unsigned int m = 0; m < pts.size(); m++)
		{
			for(unsigned int n=0; n < pts.size(); n++)
			{
				if(m!=n)
					if(sqrt((pts[m] - pts[n]).mag_squared()) < mag * 2.5)
						goto b0rk;
			}

			inc[pts[m]]=1;
			b0rk:;
		}

		srand(os);
		Image<float> a(i.size()/mag + ImageRef(1,1)), b(i.size());
		convolveGaussian(f, a, psf);
		convolveGaussian(inc, b, reconstruct_kernel);


		double n = *max_element(a.begin(), a.end());

		//for(ImageRef p(-1,0); p.next(i.size()); )
		//	a[p] /=n;

		for(ImageRef p(-1,0); p.next(a.size()); )
			a[p] = a[p] * 0.8 *(2*M_PI*psf*psf) + pic[p]*0.40 + 0.00; 

		for(ImageRef p(-1,0); p.next(a.size()); )
			a[p] += rand_g() * sqrt(a[p]) * noise;
		
		os = rand();
		n = *max_element(b.begin(), b.end());

		for(ImageRef p(-1,0); p.next(i.size()); )
			b[p] /=n;

		for(ImageRef p(-1,0); p.next(i.size()); )
			out[p] = Colourmap<Rgb<byte> >::grey(a[p/mag]);
		
		for(ImageRef p(-1,0); p.next(i.size()); )
			out[p+ImageRef(i.size().x, 0)] = Colourmap<Rgb<byte> >::hot(b[p]);
			//out[p+ImageRef(0, i.size().y)] = Colourmap<Rgb<byte> >::hot(b[p]);


		glDrawPixels(out);
		glFlush();


		img_save(out, sPrintf("delft2/out-%05i.png", nn));
	}

}
