program atom_test
    use iso_fortran_env, only: real32, real64, int32
    use chemharp
    use testing
    implicit none

    type(chrp_atom) :: atom
    real(kind=real32) :: mass = 0, charge = 0
    real(kind=real64) :: radius = 0
    integer :: status
    integer(kind=int32) :: number
    character(len=32) :: name

    call atom%init("He", status=status)
    call check((status == 0), "atom%init")

    call atom%mass(mass, status=status)
    call check((status == 0), "atom%mass")
    call check((mass == 4.002602), "atom%mass")

    call atom%charge(charge, status=status)
    call check((status == 0), "atom%charge")
    call check((charge == 0.0), "atom%charge")

    call atom%name(name, len(name), status=status)
    call check((status == 0), "atom%name")
    call check((name == "He"), "atom%name")

    call atom%set_mass(678.0, status=status)
    call check((status == 0), "atom%set_mass")
    call atom%mass(mass, status=status)
    call check((status == 0), "atom%mass")
    call check((mass == 678.0), "atom%mass")

    call atom%set_charge(-1.5, status=status)
    call check((status == 0), "atom%set_charge")
    call atom%charge(charge, status=status)
    call check((status == 0), "atom%charge")
    call check((charge == -1.5), "atom%charge")

    call atom%set_name("Zn", status=status)
    call check((status == 0), "atom%set_name")
    call atom%name(name, len(name), status=status)
    call check((status == 0), "atom%name")
    call check((name == "Zn"), "atom%name")

    call atom%full_name(name, len(name), status=status)
    call check((status == 0), "atom%full_name")
    call check((name == "Zinc"), "atom%full_name")

    call atom%vdw_radius(radius, status=status)
    call check((status == 0), "atom%vdw_radius")
    call check((radius == 2.1), "atom%vdw_radius")

    call atom%covalent_radius(radius, status=status)
    call check((status == 0), "atom%covalent_radius")
    call check((radius == 1.31), "atom%covalent_radius")

    call atom%atomic_number(number, status=status)
    call check((status == 0), "atom%atomic_number")
    call check((number == 30), "atom%atomic_number")

    call atom%free(status=status)
    call check((status == 0), "atom%free")
end program
