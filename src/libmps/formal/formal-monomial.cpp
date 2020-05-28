#include <mps/mps.h>
#include <iostream>
#include <sstream>

using namespace mps::formal;

extern "C" {

  mps_formal_monomial * 
  mps_formal_monomial_new_with_string (const char * coeff_string, long degree)
  {
    Monomial * m = new Monomial (coeff_string, degree);
    return (mps_formal_monomial*) m;
  }
  
  mps_formal_monomial *
  mps_formal_monomial_new_with_strings (const char * real, const char * imag, 
					long degree)
  {
    Monomial * m = new Monomial (real, imag, degree);
    return (mps_formal_monomial*) m;
  }

  void
  mps_formal_monomial_free (mps_formal_monomial* m)
  {
    delete ( (Monomial*) m );
  }
  
  void
  mps_formal_monomial_print (mps_formal_monomial * m)
  {
    std::cout << *((Monomial*) m);
  }

  mps_formal_monomial *
  mps_formal_monomial_neg (mps_formal_monomial * m)
  {
    Monomial *m2 = new Monomial (- *((Monomial*) m));
    return (mps_formal_monomial*) m2;
  }

  mps_formal_monomial * 
  mps_formal_monomial_mul_eq (mps_formal_monomial * m, 
			      mps_formal_monomial * other)
  {
    Monomial * mm = (Monomial*) m;
    *mm *= *((Monomial*) other);
    return (mps_formal_monomial*) mm;
  }

  mps_formal_monomial * 
  mps_formal_monomial_mul (mps_formal_monomial * m,
			   mps_formal_monomial * other)
  {
    Monomial * result = new Monomial (*((Monomial*) m) * 
				      *((Monomial*) other));
    return (mps_formal_monomial*) (result);
  }

  const char *
  mps_formal_monomial_get_str (mps_formal_monomial * m)
  {
    Monomial *mm = (Monomial *) m;
    std::stringstream output;
    output << *mm;

    char * buf = (char *) malloc(output.str().length() + 1);
    strcpy(buf, output.str().c_str());

    return buf;
  }

  int
  mps_formal_monomial_degree (mps_formal_monomial *m)
  {
    return ((Monomial*) m)->degree();
  }

}

Monomial::Monomial()
{
  mCoeffR = 0;
  mCoeffI = 0;
  mDegree = 0;
}

Monomial::Monomial(const char * coeff_string, long degree)
{
  char * er = mps_utils_build_equivalent_rational_string (NULL, coeff_string);

  mCoeffR = er;
  mDegree = degree;
  free (er);

  mCoeffR.canonicalize();
}

Monomial::Monomial(const char * real_part, const char * imag_part, long degree)
{
  char * er = mps_utils_build_equivalent_rational_string (NULL, real_part);
  char * ei = mps_utils_build_equivalent_rational_string (NULL, imag_part);

  mDegree = degree;

  mCoeffR = er;
  mCoeffI = ei;

  free (er);
  free (ei);

  mCoeffR.canonicalize();
  mCoeffI.canonicalize();
}

Monomial::Monomial(const mpq_class coeff, long degree)
{
  mCoeffR = coeff;
  mCoeffR.canonicalize();
  mDegree = degree;
}

Monomial::Monomial(const mpq_class realpart, const mpq_class imagpart, long degree)
{
  mCoeffR = realpart;
  mCoeffI = imagpart;

  mCoeffR.canonicalize();
  mCoeffI.canonicalize();

  mDegree = degree;
}

Monomial::Monomial(const Monomial& rhs)
{
  mCoeffR = rhs.coefficientReal();
  mCoeffI = rhs.coefficientImag();
  mDegree = rhs.degree();
}

Monomial::~Monomial()
{
}

bool
Monomial::isZero() const
{
  return mCoeffR == 0 && mCoeffI == 0;
}

bool
Monomial::isReal() const
{
  return mCoeffI == 0;
}

bool
Monomial::isImag() const
{
  return mCoeffR == 0;
}



Monomial
Monomial::operator-()
{
  return Monomial(-mCoeffR, -mCoeffI, mDegree);
}

Monomial& 
Monomial::operator*=(const Monomial& other)
{
  mpq_class tmp;

  tmp = mCoeffR * other.mCoeffR - mCoeffI * other.mCoeffI;
  mCoeffI = mCoeffI * other.mCoeffR + mCoeffR * other.mCoeffI;
  mCoeffR = tmp;
  mDegree += other.mDegree;

  return *this;
}

Monomial 
Monomial::operator*(const Monomial& other) const
{
  Monomial result = *this;
  result *= other;
  return result;
}

namespace mps {
  namespace formal {

    std::ostream&
    operator<<(std::ostream& os, const mps::formal::Monomial& m)
    {
      mpq_class mmCoeffI = - m.coefficientImag();
      
      if (m.isReal())
	os << m.coefficientReal();
      else if (m.coefficientReal() == 0)
	os << m.coefficientImag() << "i";
      else 
	os << "(" << m.coefficientReal() << (m.coefficientImag() > 0 ? "+" : "-")
	   << (m.coefficientImag() > 0 ? m.coefficientImag() : m.coefficientImag()) << "i)";
      
      switch (m.degree())
	{
	case 0:
	  break;
	case 1:
	  os << "x";
	  break;
	default:
	  os << "x^" << m.degree();
	  break;
	}
      
      return os;
    }
  }
}

