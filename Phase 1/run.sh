#!/bin/bash

while true; do
    clear
    echo "====== 3D Figure Generator ======"
    echo "1. Clean and Build"
    echo "2. Generate Figure"
    echo "3. View Figure"
    echo "0. Exit"
    echo "==========================================="
    
    read -p "Choose an option: " option

    case $option in
        1)
            echo "Cleaning and building..."
            if [ -d "build" ]; then
                cd build
                rm -rf *
            else
                mkdir build
                cd build
            fi
            cmake ..
            make
            cd ..
            ;;
            
        2)
            echo "=== Generate Figure ==="
            echo "1. Plane"
            echo "2. Box"
            echo "3. Sphere"
            echo "4. Cone"
            echo "0. Back"
            
            # Create tests directory in generator folder
            mkdir -p generator/tests
            
            read -p "Select figure type: " figure
            
            case $figure in
            1)
                echo "=== Plane Parameters ==="
                echo "Example: unit=2, slices=4"
                echo "Default: unit=1, slices=3"
                read -p "Enter unit size (default=1): " unit
                read -p "Enter number of slices (default=3): " slices
                unit=${unit:-1}
                slices=${slices:-3}
                cd generator
                ./generator plane $unit $slices plane.3d
                cd ..
                ;;
            2)
                echo "=== Box Parameters ==="
                echo "Example: unit=2, slices=4"
                echo "Default: unit=2, slices=3"
                read -p "Enter unit size (default=2): " unit
                read -p "Enter number of slices (default=3): " slices
                unit=${unit:-2}
                slices=${slices:-3}
                cd generator
                ./generator box $unit $slices box.3d
                cd ..
                ;;
            3)
                echo "=== Sphere Parameters ==="
                echo "Example: radius=2, slices=10, stacks=10"
                echo "Default: radius=1, slices=10, stacks=10"
                read -p "Enter radius (default=1): " radius
                read -p "Enter slices (default=10): " slices
                read -p "Enter stacks (default=10): " stacks
                radius=${radius:-1}
                slices=${slices:-10}
                stacks=${stacks:-10}
                cd generator
                ./generator sphere $radius $slices $stacks sphere.3d
                cd ..
                ;;
            4)
                echo "=== Cone Parameters ==="
                echo "Example: radius=2, height=4, slices=10, stacks=10"
                echo "Default: radius=1, height=2, slices=4, stacks=3"
                read -p "Enter radius (default=1): " radius
                read -p "Enter height (default=2): " height
                read -p "Enter slices (default=4): " slices
                read -p "Enter stacks (default=3): " stacks
                radius=${radius:-1}
                height=${height:-2}
                slices=${slices:-4}
                stacks=${stacks:-3}
                cd generator
                ./generator cone $radius $height $slices $stacks cone.3d
                cd ..
                ;;
                        0)
                            continue
                            ;;
                    esac
                    ;;
            
       3)
            echo "=== View Figure ==="
            echo "Generated figures: "
            
            for file in generator/tests/*.3d; do
                filename=$(basename "$file" .3d)
                echo "$filename"  # Removed -n flag and added newline
            done
            
            echo "0. Back to main menu"
            
            read -p "Enter the figure name (without .3d) or 0 to go back: " figure_name
            
            case $figure_name in
                0)
                    continue
                    ;;
                *)
                    cd build
                    ./engine ../engine/configs/${figure_name}_config.xml
                    cd ..
                    ;;
            esac
            ;;
            
        0)
            echo "Exiting..."
            exit 0
            ;;
            
        *)
            echo "Invalid option"
            ;;
    esac
    
    read -p "Press Enter to continue..."
done