//  Copyright John Maddock 2007.
//  Copyright Paul A. Bristow 2007
//  Use, modification and distribution are subject to the
//  Boost Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_STATS_PARETO_HPP
#define BOOST_STATS_PARETO_HPP

// http://en.wikipedia.org/wiki/Pareto_distribution
// http://www.itl.nist.gov/div898/handbook/eda/section3/eda3661.htm
// Also:
// Weisstein, Eric W. "Pareto Distribution."
// From MathWorld--A Wolfram Web Resource.
// http://mathworld.wolfram.com/ParetoDistribution.html

#include <boost/math/distributions/fwd.hpp>
#include <boost/math/distributions/complement.hpp>
#include <boost/math/distributions/detail/common_error_handling.hpp>
#include <boost/math/special_functions/powm1.hpp>

#include <utility> // for BOOST_CURRENT_VALUE?

namespace boost
{
  namespace math
  {
    namespace detail
    { // Parameter checking.
      template <class RealType, class Policy>
      inline bool check_pareto_location(
        const char* function,
        RealType location,
        RealType* result, const Policy& pol)
      {
        if((boost::math::isfinite)(location))
        { // any > 0 finite value is OK.
          if (location > 0)
          {
            return true;
          }
          else
          {
            *result = policies::raise_domain_error<RealType>(
              function,
              "Location parameter is %1%, but must be > 0!", location, pol);
            return false;
          }
        }
        else
        { // Not finite.
          *result = policies::raise_domain_error<RealType>(
            function,
            "Location parameter is %1%, but must be finite!", location, pol);
          return false;
        }
      } // bool check_pareto_location

      template <class RealType, class Policy>
      inline bool check_pareto_shape(
        const char* function,
        RealType shape,
        RealType* result, const Policy& pol)
      {
        if((boost::math::isfinite)(shape))
        { // Any finite value is OK.
          if (shape > 0)
          {
            return true;
          }
          else
          {
            *result = policies::raise_domain_error<RealType>(
              function,
              "Shape parameter is %1%, but must be > 0!", shape, pol);
            return false;
          }
        }
        else
        { // Not finite.
          *result = policies::raise_domain_error<RealType>(
            function,
            "Shape parameter is %1%, but must be finite!", shape, pol);
          return false;
        }
      } // bool check_pareto_shape(

      template <class RealType, class Policy>
      inline bool check_pareto_x(
        const char* function,
        RealType const& x,
        RealType* result, const Policy& pol)
      {
        if((boost::math::isfinite)(x))
        { // 
          if (x > 0)
          {
            return true;
          }
          else
          {
            *result = policies::raise_domain_error<RealType>(
              function,
              "x parameter is %1%, but must be > 0 !", x, pol);
            return false;
          }
        }
        else
        { // Not finite..
          *result = policies::raise_domain_error<RealType>(
            function,
            "x parameter is %1%, but must be finite!", x, pol);
          return false;
        }
      } // bool check_pareto_x

      template <class RealType, class Policy>
      inline bool check_pareto( // distribution parameters.
        const char* function,
        RealType location,
        RealType shape,
        RealType* result, const Policy& pol)
      {
        return check_pareto_location(function, location, result, pol) 
           && check_pareto_shape(function, shape, result, pol);
      } // bool check_pareto(

    } // namespace detail

    template <class RealType = double, class Policy = policies::policy<> >
    class pareto_distribution
    {
    public:
      typedef RealType value_type;
      typedef Policy policy_type;

      pareto_distribution(RealType location = 1, RealType shape = 1)
        : m_location(location), m_shape(shape)
      { // Constructor.
        RealType result;
        detail::check_pareto("boost::math::pareto_distribution<%1%>::pareto_distribution", location, shape, &result, Policy());
      }

      RealType location()const
      { // AKA Xm and b
        return m_location;
      }

      RealType shape()const
      { // AKA k and a
        return m_shape;
      }
    private:
      // Data members:
      RealType m_location;  // distribution location (xm)
      RealType m_shape;  // distribution shape (k)
    };

    typedef pareto_distribution<double> pareto; // Convenience to allow pareto(2., 3.);

    template <class RealType, class Policy>
    inline const std::pair<RealType, RealType> range(const pareto_distribution<RealType, Policy>& /*dist*/)
    { // Range of permissible values for random variable x.
      using boost::math::tools::max_value;
      return std::pair<RealType, RealType>(0, max_value<RealType>()); // location zero to + infinity.
    } // range

    template <class RealType, class Policy>
    inline const std::pair<RealType, RealType> support(const pareto_distribution<RealType, Policy>& dist)
    { // Range of supported values for random variable x.
      // This is range where cdf rises from 0 to 1, and outside it, the pdf is zero.
      using boost::math::tools::max_value;
      return std::pair<RealType, RealType>(dist.location(), max_value<RealType>() ); // location to + infinity.
    } // support

    template <class RealType, class Policy>
    inline RealType pdf(const pareto_distribution<RealType, Policy>& dist, const RealType& x)
    {
      BOOST_MATH_STD_USING  // for ADL of std function pow.
      static const char* function = "boost::math::pdf(const pareto_distribution<%1%>&, %1%)";
      RealType location = dist.location();
      RealType shape = dist.shape();
      RealType result;
      if(false == (detail::check_pareto_x(function, x, &result, Policy())
         && detail::check_pareto(function, location, shape, &result, Policy())))
         return result;
      if (x < location)
      { // regardless of shape, pdf is zero.
        return 0; 
      }

      result = shape * pow(location, shape) / pow(x, shape+1);
      return result;
    } // pdf

    template <class RealType, class Policy>
    inline RealType cdf(const pareto_distribution<RealType, Policy>& dist, const RealType& x)
    {
      BOOST_MATH_STD_USING  // for ADL of std function pow.
      static const char* function = "boost::math::cdf(const pareto_distribution<%1%>&, %1%)";
      RealType location = dist.location();
      RealType shape = dist.shape();
      RealType result;

      if(false == (detail::check_pareto_x(function, x, &result, Policy())
         && detail::check_pareto(function, location, shape, &result, Policy())))
         return result;

      if (x <= location)
      { // regardless of shape, cdf is zero.
        return 0; 
      }

      // result = RealType(1) - pow((location / x), shape);
      result = -powm1(location/x, shape, Policy()); // should be more accurate.
      return result;
    } // cdf

    template <class RealType, class Policy>
    inline RealType quantile(const pareto_distribution<RealType, Policy>& dist, const RealType& p)
    {
      BOOST_MATH_STD_USING  // for ADL of std function pow.
      static const char* function = "boost::math::quantile(const pareto_distribution<%1%>&, %1%)";
      RealType result;
      RealType location = dist.location();
      RealType shape = dist.shape();
      if(false == (detail::check_probability(function, p, &result, Policy())
           && detail::check_pareto(function, location, shape, &result, Policy())))
      {
        return result;
      }
      if (p == 0)
      {
        return location; // x must be location (or less).
      }
      if (p == 1)
      {
        return tools::max_value<RealType>(); // x = + infinity.
      }
      result = location /
        (pow((1 - p), 1 / shape));
      // K. Krishnamoorthy,  ISBN 1-58488-635-8 eq 23.1.3
      return result;
    } // quantile

    template <class RealType, class Policy>
    inline RealType cdf(const complemented2_type<pareto_distribution<RealType, Policy>, RealType>& c)
    {
       BOOST_MATH_STD_USING  // for ADL of std function pow.
       static const char* function = "boost::math::cdf(const pareto_distribution<%1%>&, %1%)";
       RealType result;
       RealType x = c.param;
       RealType location = c.dist.location();
       RealType shape = c.dist.shape();
       if(false == (detail::check_pareto_x(function, x, &result, Policy())
           && detail::check_pareto(function, location, shape, &result, Policy())))
         return result;

       if (x <= location)
       { // regardless of shape, cdf is zero, and complement is unity.
         return 1; 
       }
       result = pow((location/x), shape);
   
       return result;
    } // cdf complement
    
    template <class RealType, class Policy>
    inline RealType quantile(const complemented2_type<pareto_distribution<RealType, Policy>, RealType>& c)
    {
      BOOST_MATH_STD_USING  // for ADL of std function pow.
      static const char* function = "boost::math::quantile(const pareto_distribution<%1%>&, %1%)";
      RealType result;
      RealType q = c.param;
      RealType location = c.dist.location();
      RealType shape = c.dist.shape();
      if(false == (detail::check_probability(function, q, &result, Policy())
           && detail::check_pareto(function, location, shape, &result, Policy())))
      {
        return result;
      }
      if (q == 1)
      {
        return location; // x must be location (or less).
      }
      if (q == 0)
      {
        return tools::max_value<RealType>(); // x = + infinity.
      }
      result = location / (pow(q, 1 / shape));
      // K. Krishnamoorthy,  ISBN 1-58488-635-8 eq 23.1.3
      return result;
    } // quantile complement

    template <class RealType, class Policy>
    inline RealType mean(const pareto_distribution<RealType, Policy>& dist)
    {
      RealType result;
      static const char* function = "boost::math::mean(const pareto_distribution<%1%>&, %1%)";
      if(false == detail::check_pareto(function, dist.location(), dist.shape(), &result, Policy()))
      {
        return result;
      }
      if (dist.shape() > RealType(1))
      {
        return dist.shape() * dist.location() / (dist.shape() - 1);
      }
      else
      {
        using boost::math::tools::max_value;
        return max_value<RealType>(); // +infinity.
      }
    } // mean

    template <class RealType, class Policy>
    inline RealType mode(const pareto_distribution<RealType, Policy>& dist)
    {
      return dist.location();
    } // mode

    template <class RealType, class Policy>
    inline RealType median(const pareto_distribution<RealType, Policy>& dist)
    {
      RealType result;
      static const char* function = "boost::math::median(const pareto_distribution<%1%>&, %1%)";
      if(false == detail::check_pareto(function, dist.location(), dist.shape(), &result, Policy()))
      {
        return result;
      }
      BOOST_MATH_STD_USING
      return dist.location() * pow(RealType(2), (1/dist.shape()));
    } // median

    template <class RealType, class Policy>
    inline RealType variance(const pareto_distribution<RealType, Policy>& dist)
    {
      RealType result;
      RealType location = dist.location();
      RealType shape = dist.shape();
      static const char* function = "boost::math::variance(const pareto_distribution<%1%>&, %1%)";
      if(false == detail::check_pareto(function, location, shape, &result, Policy()))
      {
        return result;
      }
      if (shape > 2)
      {
        result = (location * location * shape) /
         ((shape - 1) *  (shape - 1) * (shape - 2));
      }
      else
      {
        result = policies::raise_domain_error<RealType>(
          function,
          "variance is undefined for shape <= 2, but got %1%.", dist.shape(), Policy());
      }
      return result;
    } // variance

    template <class RealType, class Policy>
    inline RealType skewness(const pareto_distribution<RealType, Policy>& dist)
    {  
      BOOST_MATH_STD_USING
      RealType result;
      RealType shape = dist.shape();
      static const char* function = "boost::math::pdf(const pareto_distribution<%1%>&, %1%)";
      if(false == detail::check_pareto(function, dist.location(), shape, &result, Policy()))
      {
        return result;
      }
      if (shape > 3)
      {
        result = sqrt((shape - 2) / shape) *
          2 * (shape + 1) /
          (shape - 3);
      }
      else
      {
        result = policies::raise_domain_error<RealType>(
          function,
          "skewness is undefined for shape <= 3, but got %1%.", dist.shape(), Policy());
      }
      return result;
    } // skewness

    template <class RealType, class Policy>
    inline RealType kurtosis(const pareto_distribution<RealType, Policy>& dist)
    {
      RealType result;
      RealType shape = dist.shape();
      static const char* function = "boost::math::pdf(const pareto_distribution<%1%>&, %1%)";
      if(false == detail::check_pareto(function, dist.location(), shape, &result, Policy()))
      {
        return result;
      }
      if (shape > 4)
      {
        result = 3 * ((shape - 2) * (3 * shape * shape + shape + 2)) /
          (shape * (shape - 3) * (shape - 4));
      }
      else
      {
        result = policies::raise_domain_error<RealType>(
          function,
          "kurtosis_excess is undefined for shape <= 4, but got %1%.", shape, Policy());
      }
      return result;
    } // kurtosis

    template <class RealType, class Policy>
    inline RealType kurtosis_excess(const pareto_distribution<RealType, Policy>& dist)
    {
      RealType result;
      RealType shape = dist.shape();
      static const char* function = "boost::math::pdf(const pareto_distribution<%1%>&, %1%)";
      if(false == detail::check_pareto(function, dist.location(), shape, &result, Policy()))
      {
        return result;
      }
      if (shape > 4)
      {
        result = 6 * ((shape * shape * shape) + (shape * shape) - 6 * shape - 2) /
          (shape * (shape - 3) * (shape - 4));
      }
      else
      {
        result = policies::raise_domain_error<RealType>(
          function,
          "kurtosis_excess is undefined for shape <= 4, but got %1%.", dist.shape(), Policy());
      }
      return result;
    } // kurtosis_excess

    } // namespace math
  } // namespace boost

  // This include must be at the end, *after* the accessors
  // for this distribution have been defined, in order to
  // keep compilers that support two-phase lookup happy.
#include <boost/math/distributions/detail/derived_accessors.hpp>

#endif // BOOST_STATS_PARETO_HPP


