use bib1::Leitor;

fn menor_angulo(h: i64, m: i64) -> f64 {
    // Hour hand: moves 0.5 degrees per minute total
    // At H:M, hour hand is at (H % 12) * 30 + M * 0.5 degrees
    let hour_angle = (h % 12) as f64 * 30.0 + m as f64 * 0.5;
    // Minute hand: moves 6 degrees per minute
    let minute_angle = m as f64 * 6.0;
    let mut angle = (hour_angle - minute_angle).abs();
    if angle > 180.0 {
        angle = 360.0 - angle;
    }
    angle
}

fn main() {
    let mut leitor = Leitor::new();
    loop {
        let h: i64 = match leitor.entrada() {
            Some(s) => match s.parse() {
                Ok(v) => v,
                Err(_) => break,
            },
            None => break,
        };
        let m: i64 = match leitor.entrada() {
            Some(s) => match s.parse() {
                Ok(v) => v,
                Err(_) => break,
            },
            None => break,
        };
        let result = menor_angulo(h, m);
        if result == result.floor() && result.is_finite() {
            println!("{:.1}", result);
        } else {
            println!("{}", result);
        }
    }
}
