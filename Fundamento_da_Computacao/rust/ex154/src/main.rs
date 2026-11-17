use bib1::Leitor;

fn calc_dist(v: f64, a: f64, t: f64) -> f64 {
    // With general acceleration, the object may decelerate to 0 velocity
    // then stop (velocity can't go negative since it's distance traveled)
    // v(t) = v + a*t, stops when v(t) = 0 => t_stop = -v/a (only if a < 0)
    if a >= 0.0 {
        v * t + 0.5 * a * t * t
    } else {
        let t_stop = -v / a;
        if t <= t_stop {
            // Object hasn't stopped yet
            v * t + 0.5 * a * t * t
        } else {
            // Object stopped at t_stop, then starts moving backward
            // Distance at stop
            let d_stop = v * t_stop + 0.5 * a * t_stop * t_stop;
            // After stop, the object reverses: distance = d_stop + |displacement after stop|
            let dt = t - t_stop;
            d_stop + 0.5 * a.abs() * dt * dt
        }
    }
}

fn print_float(val: f64) {
    if val == val.floor() && val.is_finite() {
        println!("{:.1}", val);
    } else {
        println!("{}", val);
    }
}

fn main() {
    let mut leitor = Leitor::new();
    loop {
        let v: f64 = match leitor.entrada() {
            Some(s) => match s.parse() {
                Ok(val) => val,
                Err(_) => break,
            },
            None => break,
        };
        let a: f64 = match leitor.entrada() {
            Some(s) => match s.parse() {
                Ok(val) => val,
                Err(_) => break,
            },
            None => break,
        };
        let t: f64 = match leitor.entrada() {
            Some(s) => match s.parse() {
                Ok(val) => val,
                Err(_) => break,
            },
            None => break,
        };
        let result = calc_dist(v, a, t);
        print_float(result);
    }
}
