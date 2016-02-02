#include <cvd/image_io.h>
#include <cvd/colourmap.h>
#include <cvd/vision.h>
#include <cvd/videodisplay.h>
#include <cvd/convolution.h>
#include <cvd/gl_helpers.h>
#include <algorithm>
#include <random>
#include <iomanip>

using namespace std;
using namespace CVD;
int main(int argc, char** argv)
{



	std::default_random_engine rng;
	std::normal_distribution<double> rand_g(0.0,1.0);

	fstream fin;
	
	int arg=1;
	if(arg >= argc)
	{
		cerr << "Error. vid output_dir/" << endl;
	}
	string dir=argv[arg];

	//Image<Rgba<byte> > i = img_load("blue-logo_cleo_2015.bmp.png");
	//Image<Rgba<byte> > i = img_load("Kings-college-london-logo-2.png");
	Image<Rgba<byte> > i = img_load(cin);
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
		
	
	Image<Rgb<byte> >  out(ImageRef(i.size().x, i.size().y*2));
	//Image<Rgb<byte> >  out(ImageRef(i.size().x*2, i.size().y));
	//VideoDisplay d(out.size());

	for(int nn=0;nn<1000;nn++)
	{
		Image<float> f(i.size() / mag + ImageRef(1,1), 0);
		for(int i=0; i < 20; i++)
		{
			ImageRef p = ps[rand()%ps.size()];
			f[p/mag]++;
			inc[p]++;
		}

		Image<float> a(i.size()/mag + ImageRef(1,1)), b(i.size());
		convolveGaussian(f, a, psf);
		convolveGaussian(inc, b, reconstruct_kernel);


		double n = *max_element(a.begin(), a.end());

		//for(ImageRef p(-1,0); p.next(i.size()); )
		//	a[p] /=n;

		for(ImageRef p(-1,0); p.next(a.size()); )
			a[p] = a[p] * 0.8 *(2*M_PI*psf*psf) + pic[p]*0.40 + 0.00; 

		for(ImageRef p(-1,0); p.next(a.size()); )
			a[p] += rand_g(rng) * sqrt(a[p]) * noise;

		n = *max_element(b.begin(), b.end());

		for(ImageRef p(-1,0); p.next(i.size()); )
			b[p] /=n;

		for(ImageRef p(-1,0); p.next(i.size()); )
			out[p] = Colourmap<Rgb<byte> >::grey(a[p/mag]);
		
		for(ImageRef p(-1,0); p.next(i.size()); )
			//out[p+ImageRef(i.size().x, 0)] = Colourmap<Rgb<byte> >::hot(b[p]);
			out[p+ImageRef(0, i.size().y)] = Colourmap<Rgb<byte> >::hot(b[p]);


		//glDrawPixels(out);
		//glFlush();

		ostringstream fn;
		fn << dir << "/out-" << setw(5) << setfill('0') << nn << setw(0) << ".png";
		img_save(out, fn.str());
	}

}
