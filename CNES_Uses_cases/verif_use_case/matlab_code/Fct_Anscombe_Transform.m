
function AT_output = Fct_Anscombe_Transform(Image,coeff_a,coeff_b)

    AT_output = (2/coeff_b)*sqrt(coeff_b*Image + 3*coeff_b^2/8 + coeff_a^2);

end