/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | Copyright (C) 2012-2016 OpenFOAM Foundation
     \\/     M anipulation  |
-------------------------------------------------------------------------------
License
    This file is part of OpenFOAM.

    OpenFOAM is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    OpenFOAM is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with OpenFOAM.  If not, see <http://www.gnu.org/licenses/>.

\*---------------------------------------------------------------------------*/

#include "perfectFluid.H"
#include "specie.H"

// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //

template<class Specie>
inline Foam::perfectFluid<Specie>::perfectFluid
(
    const Specie& sp,
    const scalar R,
    const scalar rho0
)
:
    Specie(sp),
    R_(R),
    rho0_(rho0)
{}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

template<class Specie>
inline Foam::perfectFluid<Specie>::perfectFluid
(
    const word& name,
    const perfectFluid<Specie>& pf
)
:
    Specie(name, pf),
    R_(pf.R_),
    rho0_(pf.rho0_)
{}


template<class Specie>
inline Foam::autoPtr<Foam::perfectFluid<Specie>>
Foam::perfectFluid<Specie>::clone() const
{
    return autoPtr<perfectFluid<Specie>>(new perfectFluid<Specie>(*this));
}


template<class Specie>
inline Foam::autoPtr<Foam::perfectFluid<Specie>>
Foam::perfectFluid<Specie>::New(Istream& is)
{
    return autoPtr<perfectFluid<Specie>>(new perfectFluid<Specie>(is));
}


template<class Specie>
inline Foam::autoPtr<Foam::perfectFluid<Specie>>
Foam::perfectFluid<Specie>::New
(
    const dictionary& dict
)
{
    return autoPtr<perfectFluid<Specie>>(new perfectFluid<Specie>(dict));
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template<class Specie>
inline Foam::scalar Foam::perfectFluid<Specie>::R() const
{
    return R_;
}


template<class Specie>
inline Foam::scalar Foam::perfectFluid<Specie>::rho(scalar p, scalar T) const
{
    return rho0_ + p/(this->R()*T);
}


template<class Specie>
inline Foam::scalar Foam::perfectFluid<Specie>::h(scalar p, scalar T) const
{
    return 0;
}


template<class Specie>
inline Foam::scalar Foam::perfectFluid<Specie>::cp(scalar p, scalar T) const
{
    return 0;
}


template<class Specie>
inline Foam::scalar Foam::perfectFluid<Specie>::s(scalar p, scalar T) const
{
    return -RR*log(p/Pstd);
}


template<class Specie>
inline Foam::scalar Foam::perfectFluid<Specie>::psi(scalar p, scalar T) const
{
    return 1.0/(this->R()*T);
}


template<class Specie>
inline Foam::scalar Foam::perfectFluid<Specie>::Z(scalar p, scalar T) const
{
    return 1;
}


template<class Specie>
inline Foam::scalar Foam::perfectFluid<Specie>::cpMcv(scalar p, scalar T) const
{
    return 0;
}


// * * * * * * * * * * * * * * * Member Operators  * * * * * * * * * * * * * //

template<class Specie>
inline void Foam::perfectFluid<Specie>::operator+=
(
    const perfectFluid<Specie>& pf
)
{
    scalar molr1 = this->nMoles();

    Specie::operator+=(pf);

    molr1 /= this->nMoles();
    scalar molr2 = pf.nMoles()/this->nMoles();

    R_ = 1.0/(molr1/R_ + molr2/pf.R_);
    rho0_ = molr1*rho0_ + molr2*pf.rho0_;
}


template<class Specie>
inline void Foam::perfectFluid<Specie>::operator-=
(
    const perfectFluid<Specie>& pf
)
{
    scalar molr1 = this->nMoles();

    Specie::operator-=(pf);

    molr1 /= this->nMoles();
    scalar molr2 = pf.nMoles()/this->nMoles();

    R_ = 1.0/(molr1/R_ - molr2/pf.R_);
    rho0_ = molr1*rho0_ - molr2*pf.rho0_;
}


template<class Specie>
inline void Foam::perfectFluid<Specie>::operator*=(const scalar s)
{
    Specie::operator*=(s);
}


// * * * * * * * * * * * * * * * Friend Operators  * * * * * * * * * * * * * //

template<class Specie>
inline Foam::perfectFluid<Specie> Foam::operator+
(
    const perfectFluid<Specie>& pf1,
    const perfectFluid<Specie>& pf2
)
{
    scalar nMoles = pf1.nMoles() + pf2.nMoles();
    scalar molr1 = pf1.nMoles()/nMoles;
    scalar molr2 = pf2.nMoles()/nMoles;

    return perfectFluid<Specie>
    (
        static_cast<const Specie&>(pf1)
      + static_cast<const Specie&>(pf2),
        1.0/(molr1/pf1.R_ + molr2/pf2.R_),
        molr1*pf1.rho0_ + molr2*pf2.rho0_
    );
}


template<class Specie>
inline Foam::perfectFluid<Specie> Foam::operator-
(
    const perfectFluid<Specie>& pf1,
    const perfectFluid<Specie>& pf2
)
{
    scalar nMoles = pf1.nMoles() + pf2.nMoles();
    scalar molr1 = pf1.nMoles()/nMoles;
    scalar molr2 = pf2.nMoles()/nMoles;

    return perfectFluid<Specie>
    (
        static_cast<const Specie&>(pf1)
      - static_cast<const Specie&>(pf2),
        1.0/(molr1/pf1.R_ - molr2/pf2.R_),
        molr1*pf1.rho0_ - molr2*pf2.rho0_
    );
}


template<class Specie>
inline Foam::perfectFluid<Specie> Foam::operator*
(
    const scalar s,
    const perfectFluid<Specie>& pf
)
{
    return perfectFluid<Specie>
    (
        s*static_cast<const Specie&>(pf),
        pf.R_,
        pf.rho0_
    );
}


template<class Specie>
inline Foam::perfectFluid<Specie> Foam::operator==
(
    const perfectFluid<Specie>& pf1,
    const perfectFluid<Specie>& pf2
)
{
    return pf2 - pf1;
}


// ************************************************************************* //
