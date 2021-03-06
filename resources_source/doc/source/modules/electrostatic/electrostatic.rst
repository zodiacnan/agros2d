.. _electrostatic:

Electrostatic field
===================
Electrostatics is the branch of science that deals with the phenomena and properties of stationary or slow-moving (without acceleration) electric charges.
 

Equation
^^^^^^^^
Electrostatic field can be described by Poisson partial differential equation

.. math::

   \div \varepsilon\, \grad \varphi = \rho,

where $\varepsilon$ is permittivity of the material, $\varphi$ is electrical scalar potential and $\rho$ is subdomain charge density. Electric field can be written as 

.. math::

   \vec{E} = - \grad \varphi

and electric displacement is

.. math::

   \vec{D} = \varepsilon \vec{E}.

Maxwell stress tensor:

.. math::
   \vec{S}_\mathrm{M} = \vec{E} \otimes \vec{D} - \frac{1}{2} \vec{E} \vec{D} \cdot \delta 

Boundary conditions
^^^^^^^^^^^^^^^^^^^

* Dirichlet BC

  Scalar potential $\varphi = f$ on the boundary is known.

* Neumann BC

  Surface charge density $D_\mathrm{n} = g$ on the boundary is known.

Boundary integrals
^^^^^^^^^^^^^^^^^^

Charge: 

.. math::

   Q = \int_S D_\mathrm{n} \dif S = \int_S \varepsilon \frac{\partial \varphi}{\partial n} \dif S\,\,\,\mathrm{(C)}

Subdomain integrals
^^^^^^^^^^^^^^^^^^^

Energy:

.. math::

   W_\mathrm{e} = \int_V \frac{1}{2} \vec{E} \vec{D} \dif V\,\,\,\mathrm{(J)}

Maxwell force:

.. math::
   \vec{F}_\mathrm{M} = \int_S \vec{S}_\mathrm{M} \dif S = \int_V \div \vec{S}_\mathrm{M} \dif V\,\,\,\mathrm{(N)}

.. include:: electrostatic.gen